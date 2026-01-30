#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../include/storage/buffer_pool_manager.h"
#include "../include/index/index_catalog.h"
#include "../include/index/btree/bplus_tree.h"

using namespace cmse;

int main() {

    BufferPoolManager bpm(50); // small pool to force eviction

    // Fetch metadata page (page 0)
    Page *meta_page = bpm.FetchPage(0);

    // First-time initialization
    auto *meta =
        reinterpret_cast<IndexMetaPage *>(meta_page->GetData());
    meta->index_count = 0;

    IndexCatalog catalog(meta_page);

    // Create root leaf
    PageID root_page_id;
    Page *root_page = bpm.NewPage(&root_page_id);

    auto *root_leaf =
        reinterpret_cast<BPlusTreeLeafPage *>(root_page->GetData());

    root_leaf->header.is_leaf = true;
    root_leaf->header.key_count = 0;
    root_leaf->header.parent_page_id = INVALID_PAGE_ID;
    root_leaf->next_leaf_page_id = INVALID_PAGE_ID;

    // Register index (index_id = 1)
    const IndexID TEST_INDEX_ID = 1;
    catalog.SetRoot(TEST_INDEX_ID, root_page_id);

    bpm.UnpinPage(root_page_id, true);
    bpm.UnpinPage(0, true); // metadata dirty

    BPlusTree tree(root_page_id, &bpm);

    std::cout << "Inserting keys...\n";

    const int N = 10000;
    std::srand(42);

    for (int i = 0; i < N; i++) {
        KeyType key = std::rand() % 5000;  // duplicates allowed
        RecordRef ref{static_cast<uint64_t>(i * 100)};

        tree.Insert(key, ref);
    }

    std::cout << "Insertion done.\n";

    std::cout << "\nTesting exact search...\n";

    for (int k = 0; k < 10; k++) {
        KeyType test_key = k * 100;

        std::vector<RecordRef> results;
        tree.Search(test_key, results);

        std::cout << "Key " << test_key
                  << " → " << results.size()
                  << " records\n";
    }

    std::cout << "\nTesting range search...\n";

    KeyType low = 1000;
    KeyType high = 1100;

    std::vector<RecordRef> range_results;
    tree.RangeSearch(low, high, range_results);

    std::cout << "Range [" << low << ", " << high << "] → "
              << range_results.size() << " records\n";

    bpm.FlushAllPages();

    std::cout << "\nTest finished successfully.\n";
    return 0;

}
