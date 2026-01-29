#pragma once

#include <unordered_map>
#include <vector>

#include "../common/types.h"
#include "../common/constants.h"
#include "../common/config.h"
#include "page.h"
#include "disk_manager.h"
#include "lru_replacer.h"

namespace cmse {

class BufferPoolManager {
public:
    explicit BufferPoolManager(size_t pool_size = DEFAULT_BUFFER_POOL_SIZE);
    ~BufferPoolManager();

    // Fetch the page with the given ID. Loads from disk if necessary.
    // Returns nullptr if no frame is available (pool is full and no evictable page).
    Page* FetchPage(PageID page_id);

    // Allocate a new page and return it. Assigns a new page_id via the output parameter.
    // Returns nullptr if no frame is available.
    Page* NewPage(PageID* page_id);

    // Decrease pin count. Mark dirty if is_dirty is true.
    // Returns false if the page is not in the pool or pin_count is already 0.
    bool UnpinPage(PageID page_id, bool is_dirty);

    // Flush a specific page to disk (if dirty).
    // Returns false if the page is not in the pool.
    bool FlushPage(PageID page_id);

    // Flush all dirty pages to disk (called on destruction).
    void FlushAllPages();

private:
    // Helper: allocate a frame (free or victim via LRU)
    FrameID AllocateFrame();

    const size_t pool_size_;
    Page* pages_;                                      // Array of in-memory pages (frames)
    std::unordered_map<PageID, FrameID> page_table_;   // page_id -> frame_id
    std::vector<FrameID> free_frames_;                 // List of free frames
    LRUReplacer replacer_;                             // LRU replacer for eviction
    DiskManager disk_manager_;                         // Owns the disk interface
    PageID next_page_id_ = 0;                          // Monotonically increasing page ID
};

} // namespace cmse
