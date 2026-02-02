#include "../../include/query/query_executor.h"
#include "../../include/index/btree/bplus_tree.h"
#include <iostream>

namespace cmse {

QueryExecutor::QueryExecutor(BufferPoolManager *bpm, IndexCatalog *catalog, RefReader *reader)
    : bpm_(bpm), catalog_(catalog), reader_(reader) {}

void QueryExecutor::Execute(const Query &query) {
    PageID meta_pid =
        catalog_->GetIndexMetaPageByField(query.field_name);

    if (meta_pid == INVALID_PAGE_ID) {
        std::cout << "Index not found\n";
        return;
    }

    Page *meta_page = bpm_->FetchPage(meta_pid);
    auto *meta =
        reinterpret_cast<IndexMetaEntryPage *>(meta_page->GetData());

    std::vector<RecordRef> results;
    uint32_t temp = 0;

    if (meta->index_type == IndexType::BTREE) {
        BPlusTree tree(meta->root_page_id, meta->index_id, catalog_, bpm_);

        if (query.op == QueryOp::EQUALS) {
            tree.Search(query.num_value, results, temp);
        } else if (query.op == QueryOp::BETWEEN) {
            tree.RangeSearch(query.low, query.high, results, temp);
        }
    }

    else if (meta->index_type == IndexType::TRIE) {
        TrieIndex trie(meta->root_page_id, bpm_);

        if (query.op == QueryOp::EQUALS) {
            trie.ExactSearch(query.str_value, results);
        } else if (query.op == QueryOp::STARTSWITH) {
            trie.PrefixSearch(query.str_value, results);
        }
    }

    bpm_->UnpinPage(meta_pid, false);

    // Output records
    for (auto &ref : results) {
        std::cout << reader_->Read(ref) << "\n";
    }

    std::cout << "Total results: " << results.size() << "\n";
}

} // namespace cmse

