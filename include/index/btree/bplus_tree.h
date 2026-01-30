#pragma once

#include <vector>
#include "../../common/types.h"
#include "../../common/constants.h"
#include "../../storage/buffer_pool_manager.h"

namespace cmse {

using KeyType = uint64_t;

struct RecordRef {
    uint64_t offset;   // byte offset in the log file
};

struct BPlusTreePageHeader {
    bool is_leaf;
    uint16_t key_count;
    PageID parent_page_id;
};


struct BPlusTreeLeafPage {
    BPlusTreePageHeader header;
    PageID next_leaf_page_id;
    KeyType keys[BPLUS_TREE_LEAF_MAX_KEYS];
    RecordRef values[BPLUS_TREE_LEAF_MAX_KEYS];
};

struct BPlusTreeInternalPage {
    BPlusTreePageHeader header;
    PageID children[BPLUS_TREE_INTERNAL_MAX_KEYS + 1];
    KeyType keys[BPLUS_TREE_INTERNAL_MAX_KEYS];
};


class BPlusTree {
public:
    BPlusTree(PageID root_page_id, BufferPoolManager *bpm);

    // exact match search
    void Search(KeyType key, std::vector<RecordRef> &result);

    // range match search
    void RangeSearch(KeyType low, KeyType high, std::vector<RecordRef> &result);

    // insert key
    void Insert(KeyType key, RecordRef value);

private:
    PageID FindLeafPage(KeyType key);
    PageID SplitLeaf(PageID leaf_page_id);
    PageID SplitInternal(PageID internal_page_id);

    void InsertIntoLeaf(BPlusTreeLeafPage *leaf, KeyType key, const RecordRef &value);
    void InsertIntoParent(PageID left, KeyType key, PageID right);
    void InsertIntoInternal(PageID parent_id, PageID left_child, KeyType key, PageID right_child);

    PageID root_page_id_;
    BufferPoolManager *bpm_;
};

} // namespace cmse

