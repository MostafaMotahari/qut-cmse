#pragma once

#include <cstdint>

namespace cmse {

// Logical identifier of a page on disk
using PageID = uint64_t;

// Physical slot identifier in buffer pool memory
using FrameID = uint32_t;
using KeyType = uint64_t;
using IndexID = uint32_t;

} // namespace cmse
