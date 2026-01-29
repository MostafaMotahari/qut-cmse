#include <iostream>
#include <cstring>
#include "../include/storage/buffer_pool_manager.h"

using namespace std;
using namespace cmse;

int main() {
    // IMPORTANT: Before running the program:
    // 1. Create the directory: mkdir -p data/disk
    // 2. Delete any old disk file: rm data/disk/cmse.disk   (or delete manually)
    // This ensures a clean start for the test.

    cout << "=== Starting BufferPoolManager Test ===" << endl;

    {
        // Phase 1: Create 6 pages with a small pool (3 frames) → forces evictions
        BufferPoolManager bpm(3);

        PageID pids[6];

        for (int i = 0; i < 6; ++i) {
            Page* page = bpm.NewPage(&pids[i]);
            if (page == nullptr) {
                cout << "ERROR: NewPage failed for page " << i << " (pool full and no evictable frame?)" << endl;
                return 1;
            }

            // Write identifiable data (short string, rest of page is zeroed)
            char* data = page->GetData();
            sprintf(data, "Test data for page %d - CMSE", i);

            cout << "Created page ID " << pids[i] << " and wrote data" << endl;

            // Unpin + mark dirty so it can be evicted and written back
            bpm.UnpinPage(pids[i], true);
        }

        cout << "Finished creating pages. Evictions should have occurred (oldest pages written to disk)." << endl;
    }  // <-- bpm destroyed here → FlushAllPages() writes remaining dirty pages

    cout << "First BufferPoolManager destroyed. All dirty pages flushed to disk." << endl;

    {
        // Phase 2: New BufferPoolManager → fetch the same pages from disk and verify data
        BufferPoolManager bpm(3);

        for (int i = 0; i < 6; ++i) {
            Page* page = bpm.FetchPage(i);
            if (page == nullptr) {
                cout << "ERROR: FetchPage failed for page " << i << endl;
                return 1;
            }

            const char* data = page->GetData();
            char expected[100];
            sprintf(expected, "Test data for page %d - CMSE", i);

            if (strcmp(data, expected) != 0) {
                cout << "ERROR: Data mismatch on page " << i << "!" << endl;
                cout << "   Expected: " << expected << endl;
                cout << "   Got     : " << data << endl;
                return 1;
            }

            cout << "SUCCESS: Verified page " << i << " data is correct" << endl;

            bpm.UnpinPage(i, false);  // No modify this time
        }
    }

    cout << "=== All tests passed! BufferPoolManager works correctly ===" << endl;
    return 0;
}
