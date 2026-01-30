#pragma once

#include <cstddef>
#include <cstdint>

namespace cmse {

// ================================
// Storage constants
// ================================

// Size of a disk page (bytes)
constexpr size_t PAGE_SIZE = 4096;

// ================================
// Invalid identifiers
// ================================

constexpr uint64_t INVALID_PAGE_ID = static_cast<uint64_t>(-1);
constexpr uint32_t INVALID_FRAME_ID = static_cast<uint32_t>(-1);

// ================================
// Limits (safety / sanity)
// ================================

constexpr uint32_t MAX_PIN_COUNT = 1'000'000;
constexpr uint32_t MAX_INDEXES = 16;

// ================================
// B+Tree limitations
// ================================
//
constexpr size_t BPLUS_TREE_LEAF_MAX_KEYS = 254;
constexpr size_t BPLUS_TREE_INTERNAL_MAX_KEYS = 254;

} // namespace cmse
