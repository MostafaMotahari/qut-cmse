#include "index_catalog.h"

namespace cmse {

IndexCatalog::IndexCatalog(Page* meta_page) {
    meta_ = reinterpret_cast<IndexMetaPage*>(meta_page->GetData());
}

uint32_t IndexCatalog::GetIndexCount() const {
    return meta_->index_count;
}

bool IndexCatalog::HasIndex(IndexID index_id) const {
    for (uint32_t i = 0; i < meta_->index_count; i++) {
        if (meta_->entries[i].index_id == index_id) {
            return true;
        }
    }
    return false;
}

PageID IndexCatalog::GetRoot(IndexID index_id) const {
    for (uint32_t i = 0; i < meta_->index_count; i++) {
        if (meta_->entries[i].index_id == index_id) {
            return meta_->entries[i].root_page_id;
        }
    }
    return INVALID_PAGE_ID;
}

void IndexCatalog::SetRoot(IndexID index_id, PageID root_page_id) {
    for (uint32_t i = 0; i < meta_->index_count; i++) {
        if (meta_->entries[i].index_id == index_id) {
            meta_->entries[i].root_page_id = root_page_id;
            return;
        }
    }

    // new index
    if (meta_->index_count < MAX_INDEXES) {
        meta_->entries[meta_->index_count].index_id = index_id;
        meta_->entries[meta_->index_count].root_page_id = root_page_id;
        meta_->index_count++;
    }
}

} // namespace cmse
