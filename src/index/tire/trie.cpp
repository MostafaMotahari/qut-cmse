#include <iostream>

#include "../../../include/index/trie/trie.h"

namespace cmse {

TrieIndex::TrieIndex(PageID root_page_id, BufferPoolManager *bpm)
    : root_page_id_(root_page_id), bpm_(bpm) {}

void TrieIndex::Insert(const std::string &sentence, RecordRef ref) {
    PageID current_id = root_page_id_;

    for (char c : sentence) {
        if (c < TRIE_MIN_CHAR || c > TRIE_MAX_CHAR) {
            continue; // or assert
        }

        uint32_t idx = CharToIndex(c);

        Page *page = bpm_->FetchPage(current_id);
        auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

        if (node->children[idx] == INVALID_PAGE_ID) {
            PageID new_id;
            Page *new_page = bpm_->NewPage(&new_id);

            auto *child =
                reinterpret_cast<TrieNodePage *>(new_page->GetData());

            for (uint32_t i = 0; i < TRIE_ALPHABET_SIZE; i++) {
                child->children[i] = INVALID_PAGE_ID;
            }

            child->is_terminal = false;
            child->record_count = 0;

            node->children[idx] = new_id;

            bpm_->UnpinPage(new_id, true);
        }

        PageID next_id = node->children[idx];
        bpm_->UnpinPage(current_id, true);
        current_id = next_id;
    }

    // terminal node
    Page *page = bpm_->FetchPage(current_id);
    auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

    node->is_terminal = true;

    if (node->record_count < TRIE_MAX_RECORDS) {
        node->records[node->record_count++] = ref;
    }

    bpm_->UnpinPage(current_id, true);
}

void TrieIndex::ExactSearch(const std::string &sentence, std::vector<RecordRef> &result) {
    result.clear();

    PageID current_id = root_page_id_;

    for (char c : sentence) {
        uint32_t idx = CharToIndex(c);

        Page *page = bpm_->FetchPage(current_id);
        auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

        if (node->children[idx] == INVALID_PAGE_ID) {
            bpm_->UnpinPage(current_id, false);
            return;
        }

        PageID next = node->children[idx];
        bpm_->UnpinPage(current_id, false);
        current_id = next;
    }

    Page *page = bpm_->FetchPage(current_id);
    auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

    if (node->is_terminal) {
        for (uint32_t i = 0; i < node->record_count; i++) {
            result.push_back(node->records[i]);
        }
    }

    bpm_->UnpinPage(current_id, false);
}

void TrieIndex::CollectAll(PageID node_id, std::vector<RecordRef> &result) {
    Page *page = bpm_->FetchPage(node_id);
    auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

    // Collect records
    if (node->is_terminal) {
        for (uint32_t i = 0; i < node->record_count; i++) {
            result.push_back(node->records[i]);
        }
    }

    // Copy children first
    PageID children[TRIE_ALPHABET_SIZE];
    for (uint32_t i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        children[i] = node->children[i];
    }

    bpm_->UnpinPage(node_id, false);

    // Recurse AFTER unpin
    for (uint32_t i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        if (children[i] != INVALID_PAGE_ID) {
            CollectAll(children[i], result);
        }
    }
}

void TrieIndex::PrefixSearch(const std::string &prefix, std::vector<RecordRef> &result) {
    result.clear();

    PageID current_id = root_page_id_;

    for (char c : prefix) {
        uint32_t idx = CharToIndex(c);

        Page *page = bpm_->FetchPage(current_id);
        auto *node = reinterpret_cast<TrieNodePage *>(page->GetData());

        if (node->children[idx] == INVALID_PAGE_ID) {
            bpm_->UnpinPage(current_id, false);
            return;
        }

        PageID next = node->children[idx];
        bpm_->UnpinPage(current_id, false);
        current_id = next;
    }

    CollectAll(current_id, result);
}

}
