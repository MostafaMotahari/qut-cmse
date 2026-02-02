#pragma once

#include "../common/types.h"
#include "../common/constants.h"

namespace cmse {

enum class IndexType : uint8_t {
    BTREE = 1,
    TRIE  = 2
};

enum class FieldType : uint8_t {
    NUMERIC = 1,
    STRING  = 2
};

struct IndexMetaEntryPage {
    IndexID index_id;

    char field_name[32];        // "timestamp", "severity", "message"
    FieldType field_type;
    IndexType index_type;

    PageID root_page_id;
};

struct IndexMetaPage {
    uint32_t index_count;
    PageID index_meta_pages[MAX_INDEXES];
};

} // namespace cmse
