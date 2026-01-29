#pragma once

#include <list>
#include <unordered_map>

#include "../common/types.h"

namespace cmse {

class LRUReplacer {
public:
    LRUReplacer();
    ~LRUReplacer() = default;

    // Remove a frame from replacer (when it is pinned)
    void Pin(FrameID frame_id);

    // Add a frame to replacer (when it becomes unpinned)
    void Unpin(FrameID frame_id);

    // Choose a victim frame to evict
    bool Victim(FrameID* frame_id);

    // Number of frames currently tracked
    size_t Size() const;

private:
    // Most recently used at front
    // Least recently used at back
    std::list<FrameID> lru_list_;

    // FrameID -> position in list
    std::unordered_map<FrameID, std::list<FrameID>::iterator> lru_map_;
};

} // namespace cmse

