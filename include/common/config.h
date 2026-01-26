#pragma once

#include <string>

namespace cmse {

// ================================
// Buffer Pool Configuration
// ================================

// Number of pages that can be held in memory
constexpr size_t DEFAULT_BUFFER_POOL_SIZE = 128;

// ================================
// Disk Configuration
// ================================

// Path to simulated disk file
inline const std::string DISK_FILE_PATH = "data/disk/cmse.disk";

// ================================
// Debug / Logging
// ================================

constexpr bool ENABLE_LOGGING = true;

} // namespace cmse

