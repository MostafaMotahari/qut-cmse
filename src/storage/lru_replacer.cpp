#include "../../include/storage/lru_replacer.h"

namespace cmse {

LRUReplacer::LRUReplacer() {}

void LRUReplacer::Pin(FrameID frame_id) {
    // If frame is in LRU, remove it
    auto it = lru_map_.find(frame_id);
    if (it != lru_map_.end()) {
        lru_list_.erase(it->second);
        lru_map_.erase(it);
    }
}

void LRUReplacer::Unpin(FrameID frame_id) {
    // If frame already exists, do nothing
    if (lru_map_.find(frame_id) != lru_map_.end()) {
        return;
    }

    // Insert at front (most recently used)
    lru_list_.push_front(frame_id);
    lru_map_[frame_id] = lru_list_.begin();
}

bool LRUReplacer::Victim(FrameID* frame_id) {
    if (lru_list_.empty()) {
        return false;
    }

    // Least recently used frame is at back
    *frame_id = lru_list_.back();

    // Remove it
    lru_map_.erase(*frame_id);
    lru_list_.pop_back();

    return true;
}

size_t LRUReplacer::Size() const {
    return lru_list_.size();
}

} // namespace cmse

