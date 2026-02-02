#include "../../../include/index/index_meta_page.h"
#include "../../../include/index/btree/bplus_tree.h"
#include "../../../include/storage/buffer_pool_manager.h"

namespace cmse {

BPlusTree::BPlusTree(PageID root_page_id, IndexID index_id, BufferPoolManager *bpm)
    : root_page_id_(root_page_id), bpm_(bpm), index_id_(index_id) {}

PageID BPlusTree::FindLeafPageForSearch(KeyType key, uint32_t &fetch_count) {
    PageID current_page_id = root_page_id_;

    while (true) {
        fetch_count++;
        Page *page = bpm_->FetchPage(current_page_id);
        auto *header =
            reinterpret_cast<BPlusTreePageHeader *>(page->GetData());

        // reached leaf
        if (header->is_leaf) {
            bpm_->UnpinPage(current_page_id, false);
            return current_page_id;
        }

        // internal page
        auto *internal =
            reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());

        // Phase 3 pruning
        if (key < internal->min_key || key > internal->max_key) {
            bpm_->UnpinPage(current_page_id, false);
            return INVALID_PAGE_ID;
        }

        uint32_t i = 0;
        while (i < internal->header.key_count && key >= internal->keys[i]) {
            i++;
        }

        PageID next_page_id = internal->children[i];

        bpm_->UnpinPage(current_page_id, false);
        current_page_id = next_page_id;
    }
}

PageID BPlusTree::FindLeafPageForInsert(KeyType key) {
    PageID current_page_id = root_page_id_;

    while (true) {
        Page *page = bpm_->FetchPage(current_page_id);
        auto *header =
            reinterpret_cast<BPlusTreePageHeader *>(page->GetData());

        // reached leaf
        if (header->is_leaf) {
            bpm_->UnpinPage(current_page_id, false);
            return current_page_id;
        }

        // internal page
        auto *internal =
            reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());

        uint32_t i = 0;
        while (i < internal->header.key_count && key >= internal->keys[i]) {
            i++;
        }

        PageID next_page_id = internal->children[i];

        bpm_->UnpinPage(current_page_id, false);
        current_page_id = next_page_id;
    }
}

void BPlusTree::Search(KeyType key, std::vector<RecordRef> &result, uint32_t &page_fetch_count) {
    result.clear();

    PageID leaf_page_id = FindLeafPageForSearch(key, page_fetch_count);
    if (leaf_page_id == INVALID_PAGE_ID) {
        return;
    }

    Page *page = bpm_->FetchPage(leaf_page_id);

    auto *leaf =
        reinterpret_cast<BPlusTreeLeafPage *>(page->GetData());

    // binary search is possible, but linear is OK for now
    for (uint32_t i = 0; i < leaf->header.key_count; i++) {
        if (leaf->keys[i] == key) {
            result.push_back(leaf->values[i]);
        }
    }

    bpm_->UnpinPage(leaf_page_id, false);
}

void BPlusTree::RangeSearch(KeyType low, KeyType high, std::vector<RecordRef> &result, uint32_t &page_fetch_count) {
    result.clear();

    // Step 1: find starting leaf
    PageID leaf_page_id = FindLeafPageForSearch(low, page_fetch_count);
    if (leaf_page_id == INVALID_PAGE_ID) {
        return;
    }

    while (leaf_page_id != INVALID_PAGE_ID) {
        page_fetch_count++;
        Page *page = bpm_->FetchPage(leaf_page_id);
        auto *leaf =
            reinterpret_cast<BPlusTreeLeafPage *>(page->GetData());

        bool should_continue = true;

        // Step 2: scan keys in current leaf
        for (uint32_t i = 0; i < leaf->header.key_count; i++) {
            KeyType key = leaf->keys[i];

            if (key < low) {
                continue;
            }

            if (key > high) {
                should_continue = false;
                break;
            }

            result.push_back(leaf->values[i]);
        }

        PageID next_leaf = leaf->next_leaf_page_id;
        bpm_->UnpinPage(leaf_page_id, false);

        // Step 3: decide whether to move to next leaf
        if (!should_continue) {
            break;
        }

        leaf_page_id = next_leaf;
    }
}

void BPlusTree::Insert(KeyType key, RecordRef value) {

    PageID leaf_page_id = FindLeafPageForInsert(key);
    Page *page = bpm_->FetchPage(leaf_page_id);
    auto *leaf =
        reinterpret_cast<BPlusTreeLeafPage *>(page->GetData());

    InsertIntoLeaf(leaf, key, value);

    PageID parent_id = leaf->header.parent_page_id;
    bpm_->UnpinPage(leaf_page_id, true);

    while (parent_id != INVALID_PAGE_ID) {
        Page *p = bpm_->FetchPage(parent_id);
        auto *internal =
            reinterpret_cast<BPlusTreeInternalPage *>(p->GetData());

        UpdateInternalStats(internal, key);

        parent_id = internal->header.parent_page_id;
        bpm_->UnpinPage(p->GetPageID(), true);
    }

    if (leaf->header.key_count > BPLUS_TREE_LEAF_MAX_KEYS) {
        SplitLeaf(leaf_page_id);
    }
}

void BPlusTree::InsertIntoLeaf(BPlusTreeLeafPage *leaf, KeyType key, const RecordRef &value) {
    uint32_t n = leaf->header.key_count;

    // find insert position
    uint32_t pos = 0;
    while (pos < n && leaf->keys[pos] < key) {
        pos++;
    }

    // shift keys & values right
    for (uint32_t i = n; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->values[i] = leaf->values[i - 1];
    }

    // insert new entry
    leaf->keys[pos] = key;
    leaf->values[pos] = value;
    leaf->header.key_count++;
}

PageID BPlusTree::SplitLeaf(PageID leaf_page_id) {
    Page *old_page = bpm_->FetchPage(leaf_page_id);
    auto *old_leaf =
        reinterpret_cast<BPlusTreeLeafPage *>(old_page->GetData());

    // 1️⃣ Allocate new leaf page
    PageID new_leaf_page_id;
    Page *new_page = bpm_->NewPage(&new_leaf_page_id);
    auto *new_leaf =
        reinterpret_cast<BPlusTreeLeafPage *>(new_page->GetData());

    // initialize new leaf
    new_leaf->header.is_leaf = true;
    new_leaf->header.key_count = 0;
    new_leaf->header.parent_page_id = old_leaf->header.parent_page_id;

    // 2️⃣ Split point
    uint32_t split_index = old_leaf->header.key_count / 2;

    // 3️⃣ Move second half to new leaf
    for (uint32_t i = split_index; i < old_leaf->header.key_count; i++) {

        new_leaf->keys[new_leaf->header.key_count] =
            old_leaf->keys[i];
        new_leaf->values[new_leaf->header.key_count] =
            old_leaf->values[i];

        new_leaf->header.key_count++;
    }

    // 4️⃣ Update old leaf key count
    old_leaf->header.key_count = split_index;

    // 5️⃣ Fix leaf linked list
    new_leaf->next_leaf_page_id = old_leaf->next_leaf_page_id;
    old_leaf->next_leaf_page_id = new_leaf_page_id;

    // 6️⃣ Promote first key of new leaf
    KeyType promote_key = new_leaf->keys[0];

    InsertIntoParent(leaf_page_id, promote_key, new_leaf_page_id);

    // unpin pages
    bpm_->UnpinPage(leaf_page_id, true);
    bpm_->UnpinPage(new_leaf_page_id, true);

    return new_leaf_page_id;
}

void BPlusTree::InsertIntoParent(PageID left, KeyType key, PageID right) {
    Page *left_page = bpm_->FetchPage(left);
    auto *left_header =
        reinterpret_cast<BPlusTreePageHeader *>(left_page->GetData());

    // Case 1: left was root
    if (left_header->parent_page_id == INVALID_PAGE_ID) {
        PageID new_root_id;
        Page *root_page = bpm_->NewPage(&new_root_id);

        auto *root =
            reinterpret_cast<BPlusTreeInternalPage *>(root_page->GetData());

        root->header.is_leaf = false;
        root->header.key_count = 1;
        root->header.parent_page_id = INVALID_PAGE_ID;

        root->children[0] = left;
        root->children[1] = right;
        root->keys[0] = key;

        // Update statistics
        root->min_key = key;
        root->max_key = key;
        root->total_keys = 1;
        root->density = 1.0f;

        // update children parent pointers
        left_header->parent_page_id = new_root_id;

        Page *right_page = bpm_->FetchPage(right);
        auto *right_header =
            reinterpret_cast<BPlusTreePageHeader *>(right_page->GetData());
        right_header->parent_page_id = new_root_id;

        root_page_id_ = new_root_id;

        // Persist new root in metadata
        Page *meta_page = bpm_->FetchPage(0);
        IndexMetaPage *meta =
            reinterpret_cast<IndexMetaPage *>(meta_page->GetData());

        for (uint32_t i = 0; i < meta->index_count; i++) {
            if (meta->entries[i].index_id == index_id_) {
                meta->entries[i].root_page_id = new_root_id;
                break;
            }
        }

        bpm_->UnpinPage(0, true);

        bpm_->UnpinPage(right, true);
        bpm_->UnpinPage(new_root_id, true);
        bpm_->UnpinPage(left, true);
        return;
    }

    // Case 2: normal internal insert
    PageID parent_id = left_header->parent_page_id;
    bpm_->UnpinPage(left, false);

    InsertIntoInternal(parent_id, left, key, right);
}

void BPlusTree::InsertIntoInternal(PageID parent_id, PageID left_child, KeyType key, PageID right_child) {
    Page *page = bpm_->FetchPage(parent_id);
    auto *internal =
        reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());

    uint32_t n = internal->header.key_count;

    // find index of left child
    uint32_t idx = 0;
    while (idx <= n && internal->children[idx] != left_child) {
        idx++;
    }

    // shift keys and children right
    for (uint32_t i = n; i > idx; i--) {
        internal->keys[i] = internal->keys[i - 1];
        internal->children[i + 1] = internal->children[i];
    }

    // insert
    internal->keys[idx] = key;
    internal->children[idx + 1] = right_child;
    internal->header.key_count++;

    // Update statistics
    UpdateInternalStats(internal, key);

    // update right child parent pointer
    Page *right_page = bpm_->FetchPage(right_child);
    auto *right_header =
        reinterpret_cast<BPlusTreePageHeader *>(right_page->GetData());
    right_header->parent_page_id = parent_id;
    bpm_->UnpinPage(right_child, true);

    // overflow?
    if (internal->header.key_count > BPLUS_TREE_INTERNAL_MAX_KEYS) {
        PageID new_internal = SplitInternal(parent_id);
        bpm_->UnpinPage(parent_id, true);
        return;
    }

    bpm_->UnpinPage(parent_id, true);
}

PageID BPlusTree::SplitInternal(PageID internal_page_id) {
    Page *old_page = bpm_->FetchPage(internal_page_id);
    auto *old =
        reinterpret_cast<BPlusTreeInternalPage *>(old_page->GetData());

    PageID new_page_id;
    Page *new_page = bpm_->NewPage(&new_page_id);
    auto *new_internal =
        reinterpret_cast<BPlusTreeInternalPage *>(new_page->GetData());

    new_internal->header.is_leaf = false;
    new_internal->header.parent_page_id = old->header.parent_page_id;
    new_internal->header.key_count = 0;
    new_internal->total_keys = 0;
    new_internal->density = 0.0f;

    uint32_t total_keys = old->header.key_count;
    uint32_t mid = total_keys / 2;

    KeyType promote_key = old->keys[mid];

    // move keys & children AFTER mid to new node
    for (uint32_t i = mid + 1; i < total_keys; i++) {
        KeyType k = old->keys[i];

        new_internal->keys[new_internal->header.key_count] = k;
        new_internal->children[new_internal->header.key_count] = old->children[i];
        new_internal->header.key_count++;

        UpdateInternalStats(new_internal, k);
    }

    // last child
    new_internal->children[new_internal->header.key_count] =
        old->children[total_keys];

    // update parent pointer of moved children
    for (uint32_t i = 0; i <= new_internal->header.key_count; i++) {
        Page *child = bpm_->FetchPage(new_internal->children[i]);
        auto *hdr =
            reinterpret_cast<BPlusTreePageHeader *>(child->GetData());
        hdr->parent_page_id = new_page_id;
        bpm_->UnpinPage(new_internal->children[i], true);
    }

    old->header.key_count = mid;
    if (old->header.key_count > 0) {
        old->min_key = old->keys[0];
        old->max_key = old->keys[old->header.key_count - 1];
    }

    old->density =
        static_cast<float>(old->total_keys) /
        static_cast<float>(old->max_key - old->min_key + 1);

    bpm_->UnpinPage(internal_page_id, true);
    bpm_->UnpinPage(new_page_id, true);

    // insert promoted key to parent
    InsertIntoParent(internal_page_id, promote_key, new_page_id);

    return new_page_id;
}

void BPlusTree::UpdateInternalStats(BPlusTreeInternalPage *node, KeyType key) {
    if (node->total_keys == 0) {
        node->min_key = key;
        node->max_key = key;
        node->total_keys = 1;
        node->density = 1.0f;
        return;
    }

    if (key < node->min_key) node->min_key = key;
    if (key > node->max_key) node->max_key = key;

    node->total_keys++;

    node->density =
        static_cast<float>(node->total_keys) /
        static_cast<float>(node->max_key - node->min_key + 1);
}

}
