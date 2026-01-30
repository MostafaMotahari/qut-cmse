#pragma once

#include "index_meta_page.h"
#include "../storage/page.h"

namespace cmse {

class IndexCatalog {
public:
    explicit IndexCatalog(Page* meta_page);

    PageID GetRoot(IndexID index_id) const;
    void SetRoot(IndexID index_id, PageID root_page_id);

    bool HasIndex(IndexID index_id) const;
    uint32_t GetIndexCount() const;

private:
    IndexMetaPage* meta_;
};

} // namespace cmse

