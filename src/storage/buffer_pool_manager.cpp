#include "../../include/storage/buffer_pool_manager.h"

#include <cstring>

namespace cmse {

BufferPoolManager::BufferPoolManager(size_t pool_size)
    : pool_size_(pool_size), disk_manager_(), replacer_() {
    pages_ = new Page[pool_size_];
    free_frames_.reserve(pool_size_);
    for (size_t i = 0; i < pool_size_; ++i) {
        free_frames_.push_back(static_cast<FrameID>(i));
    }
}

BufferPoolManager::~BufferPoolManager() {
    FlushAllPages();
    delete[] pages_;
}

FrameID BufferPoolManager::AllocateFrame() {
    FrameID frame_id = INVALID_FRAME_ID;

    // 1. Try to get a free frame
    if (!free_frames_.empty()) {
        frame_id = free_frames_.back();
        free_frames_.pop_back();
        return frame_id;
    }

    // 2. No free frame -> evict using LRU
    if (replacer_.Victim(&frame_id)) {
        PageID old_page_id = pages_[frame_id].GetPageID();

        // Write back if dirty
        if (pages_[frame_id].IsDirty()) {
            disk_manager_.WritePage(old_page_id, pages_[frame_id].GetData());
        }

        // Remove old mapping
        page_table_.erase(old_page_id);

        return frame_id;
    }

    // No frame available
    return INVALID_FRAME_ID;
}

Page* BufferPoolManager::FetchPage(PageID page_id) {
    // Case 1: Page already in buffer pool
    auto it = page_table_.find(page_id);
    if (it != page_table_.end()) {
        FrameID frame_id = it->second;
        replacer_.Pin(frame_id);        // Remove from replacer if present
        pages_[frame_id].Pin();
        return &pages_[frame_id];
    }

    // Case 2: Page not in pool -> allocate frame and load from disk
    FrameID frame_id = AllocateFrame();
    if (frame_id == INVALID_FRAME_ID) {
        return nullptr;
    }

    pages_[frame_id].Reset();
    disk_manager_.ReadPage(page_id, pages_[frame_id].GetData());
    pages_[frame_id].SetPageID(page_id);
    page_table_[page_id] = frame_id;
    pages_[frame_id].Pin();             // Caller now holds a pin

    return &pages_[frame_id];
}

Page* BufferPoolManager::NewPage(PageID* page_id) {
    FrameID frame_id = AllocateFrame();
    if (frame_id == INVALID_FRAME_ID) {
        return nullptr;
    }

    pages_[frame_id].Reset();

    PageID new_page_id = next_page_id_++;
    *page_id = new_page_id;

    pages_[frame_id].SetPageID(new_page_id);
    page_table_[new_page_id] = frame_id;
    pages_[frame_id].Pin();
    pages_[frame_id].SetDirty(true);    // New pages are considered dirty (zeroed page should persist)

    return &pages_[frame_id];
}

bool BufferPoolManager::UnpinPage(PageID page_id, bool is_dirty) {
    auto it = page_table_.find(page_id);
    if (it == page_table_.end()) {
        return false;
    }

    FrameID frame_id = it->second;
    if (pages_[frame_id].GetPinCount() == 0) {
        return false;   // Invalid unpin
    }

    if (is_dirty) {
        pages_[frame_id].SetDirty(true);
    }

    pages_[frame_id].Unpin();

    if (pages_[frame_id].GetPinCount() == 0) {
        replacer_.Unpin(frame_id);
    }

    return true;
}

bool BufferPoolManager::FlushPage(PageID page_id) {
    auto it = page_table_.find(page_id);
    if (it == page_table_.end()) {
        return false;
    }

    FrameID frame_id = it->second;
    disk_manager_.WritePage(page_id, pages_[frame_id].GetData());
    pages_[frame_id].SetDirty(false);
    return true;
}

void BufferPoolManager::FlushAllPages() {
    for (const auto& pair : page_table_) {
        PageID page_id = pair.first;
        FrameID frame_id = pair.second;
        if (pages_[frame_id].IsDirty()) {
            disk_manager_.WritePage(page_id, pages_[frame_id].GetData());
            pages_[frame_id].SetDirty(false);
        }
    }
}

} // namespace cmse
