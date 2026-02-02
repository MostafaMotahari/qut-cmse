#pragma once

#include "index_meta_page.h"
#include "../storage/buffer_pool_manager.h"

namespace cmse {

class IndexCatalog {
public:
    explicit IndexCatalog(BufferPoolManager *bpm);

    PageID GetRoot(IndexID index_id) const;
    void SetRoot(IndexID index_id, PageID root_page_id);

    bool HasIndex(IndexID index_id) const;
    uint32_t GetIndexCount() const;

    // NEW (needed for query layer later)
    PageID GetIndexMetaPage(IndexID index_id) const;
    PageID GetIndexMetaPageByField(const std::string &field_name) const;

private:
    BufferPoolManager *bpm_;
    IndexMetaPage *directory_;   // page 0
};

} // namespace cmse

