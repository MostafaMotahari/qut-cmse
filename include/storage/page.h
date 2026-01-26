#pragma once

#include <cstddef>
#include <cstring>

#include "../common/types.h"
#include "../common/constants.h"

namespace cmse {

/**
 * Page represents a fixed-size block of data
 * loaded from disk into memory.
 *
 * The buffer pool manages Page objects.
 */
class Page {
public:
    Page() {
        Reset();
    }

    // Reset page metadata and clear data buffer
    void Reset() {
        page_id_ = INVALID_PAGE_ID;
        is_dirty_ = false;
        pin_count_ = 0;
        std::memset(data_, 0, PAGE_SIZE);
    }

    // Get raw data pointer
    char* GetData() {
        return data_;
    }

    const char* GetData() const {
        return data_;
    }

    // Metadata accessors
    PageID GetPageID() const {
        return page_id_;
    }

    void SetPageID(PageID page_id) {
        page_id_ = page_id;
    }

    bool IsDirty() const {
        return is_dirty_;
    }

    void SetDirty(bool is_dirty) {
        is_dirty_ = is_dirty;
    }

    uint32_t GetPinCount() const {
        return pin_count_;
    }

    void Pin() {
        ++pin_count_;
    }

    void Unpin() {
        if (pin_count_ > 0) {
            --pin_count_;
        }
    }

private:
    PageID page_id_;
    bool is_dirty_;
    uint32_t pin_count_;

    // Actual page data
    char data_[PAGE_SIZE];
};

} // namespace cmse
