#pragma once

#include <cstddef>
#include <cstdint>
#include "../../common/constants.h"
#include "../../common/types.h"
#include "../../storage/buffer_pool_manager.h"

namespace cmse {

inline uint32_t CharToIndex(char c) {
    return static_cast<uint32_t>(c) - TRIE_MIN_CHAR;
}

struct TrieNodePage {
    // children[i] == INVALID_PAGE_ID means no edge
    PageID children[TRIE_ALPHABET_SIZE];

    bool is_terminal;          // sentence ends here

    uint32_t record_count;     // duplicates allowed
    RecordRef records[TRIE_MAX_RECORDS];
};

class TrieIndex {
public:
    TrieIndex(PageID root_page_id, BufferPoolManager *bpm);

    void Insert(const std::string &sentence, RecordRef ref);

    void ExactSearch(const std::string &sentence, std::vector<RecordRef> &result);

    void PrefixSearch(const std::string &prefix, std::vector<RecordRef> &result);

private:
    PageID root_page_id_;
    BufferPoolManager *bpm_;

    PageID FindNode(const std::string &key, bool create);
    void CollectAll(PageID node_id, std::vector<RecordRef> &result);
};

}
