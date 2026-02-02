
#include <iostream>
#include <vector>
#include <cstdlib>

#include "../include/storage/buffer_pool_manager.h"
#include "../include/index/index_catalog.h"
#include "../include/index/trie/trie.h"

using namespace cmse;

int main() {

    BufferPoolManager bpm(50); // small pool to force eviction

    // Fetch metadata page (page 0)

    IndexCatalog catalog(&bpm);

    // Create root leaf
    PageID trie_root_id;
    Page *root_page = bpm.NewPage(&trie_root_id);

    auto *root =
        reinterpret_cast<TrieNodePage *>(root_page->GetData());

    for (uint32_t i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        root->children[i] = INVALID_PAGE_ID;
    }

    root->is_terminal = false;
    root->record_count = 0;

    const IndexID TRIE_INDEX_ID = 2;

    catalog.SetRoot(TRIE_INDEX_ID, trie_root_id);

    bpm.UnpinPage(trie_root_id, true);
    bpm.UnpinPage(0, true); // meta dirty

    TrieIndex trie(trie_root_id, &bpm);

    std::vector<std::string> samples = {
        "MESSAGE=pam_unix(cron:session): session opened for user root(uid=0)",
        "MESSAGE=pam_unix(cron:session): session closed for user root",
        "ERROR=disk_failure device=sda1 sector=991823",
        "INFO=systemd started service sshd",
    };

    const int N = 10000;

    std::cout << "Inserting keys...\n";

    for (int i = 0; i < N; i++) {
        std::string s = samples[i % samples.size()];
        RecordRef ref{static_cast<uint64_t>(i * 128)};
        // std::cout<<"Hey"<<std::endl;
        trie.Insert(s, ref);
    }

    std::cout << "Insertion done.\n";

    std::vector<RecordRef> results;

    trie.ExactSearch(
        "MESSAGE=pam_unix(cron:session): session opened for user root(uid=0)",
        results
    );

    std::cout << "Exact results: " << results.size() << "\n";


    results.clear();

    trie.PrefixSearch("MESSAGE=pam_unix", results);

    std::cout << "Prefix results: " << results.size() << "\n";


    bpm.FlushAllPages();

    std::cout << "\nTest finished successfully.\n";
    return 0;

}
