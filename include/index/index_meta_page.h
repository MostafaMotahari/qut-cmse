#pragma once

#include "../common/types.h"
#include "../common/constants.h"

namespace cmse {

struct IndexMetaEntry {
    IndexID index_id;
    PageID root_page_id;
};

struct IndexMetaPage {
    uint32_t index_count;
    IndexMetaEntry entries[MAX_INDEXES];
};

} // namespace cmse
