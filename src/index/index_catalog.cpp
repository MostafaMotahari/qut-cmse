#include "../../include/index/index_catalog.h"

namespace cmse {

IndexCatalog::IndexCatalog(BufferPoolManager *bpm)
    : bpm_(bpm) {
    Page *page = bpm_->FetchPage(0);
    directory_ = reinterpret_cast<IndexMetaPage *>(page->GetData());
    // DO NOT unpin page 0 here; catalog lives long
}

uint32_t IndexCatalog::GetIndexCount() const {
    return directory_->index_count;
}

bool IndexCatalog::HasIndex(IndexID index_id) const {
    for (uint32_t i = 0; i < directory_->index_count; i++) {
        PageID meta_pid = directory_->index_meta_pages[i];
        Page *page = bpm_->FetchPage(meta_pid);

        auto *meta =
            reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

        bool found = (meta->index_id == index_id);
        bpm_->UnpinPage(meta_pid, false);

        if (found) return true;
    }
    return false;
}

PageID IndexCatalog::GetIndexMetaPage(IndexID index_id) const {
    for (uint32_t i = 0; i < directory_->index_count; i++) {
        PageID meta_pid = directory_->index_meta_pages[i];
        Page *page = bpm_->FetchPage(meta_pid);

        auto *meta =
            reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

        if (meta->index_id == index_id) {
            bpm_->UnpinPage(meta_pid, false);
            return meta_pid;
        }

        bpm_->UnpinPage(meta_pid, false);
    }
    return INVALID_PAGE_ID;
}

PageID IndexCatalog::GetRoot(IndexID index_id) const {
    PageID meta_pid = GetIndexMetaPage(index_id);
    if (meta_pid == INVALID_PAGE_ID) {
        return INVALID_PAGE_ID;
    }

    Page *page = bpm_->FetchPage(meta_pid);
    auto *meta =
        reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

    PageID root = meta->root_page_id;
    bpm_->UnpinPage(meta_pid, false);

    return root;
}

void IndexCatalog::SetRoot(IndexID index_id, PageID root_page_id) {
    // update existing index
    for (uint32_t i = 0; i < directory_->index_count; i++) {
        PageID meta_pid = directory_->index_meta_pages[i];
        Page *page = bpm_->FetchPage(meta_pid);

        auto *meta =
            reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

        if (meta->index_id == index_id) {
            meta->root_page_id = root_page_id;
            bpm_->UnpinPage(meta_pid, true);
            return;
        }

        bpm_->UnpinPage(meta_pid, false);
    }

    // create new index entry
    if (directory_->index_count < MAX_INDEXES) {
        PageID new_meta_pid;
        Page *page = bpm_->NewPage(&new_meta_pid);

        auto *meta =
            reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

        meta->index_id = index_id;
        meta->root_page_id = root_page_id;
        meta->field_name[0] = '\0';   // optional init
        meta->field_type = FieldType::NUMERIC; // default
        meta->index_type = IndexType::BTREE;   // default

        directory_->index_meta_pages[directory_->index_count] =
            new_meta_pid;
        directory_->index_count++;

        bpm_->UnpinPage(new_meta_pid, true);
        bpm_->UnpinPage(0, true); // directory dirty
    }
}

PageID IndexCatalog::GetIndexMetaPageByField(const std::string &field_name) const {
    for (uint32_t i = 0; i < directory_->index_count; i++) {
        PageID meta_pid = directory_->index_meta_pages[i];
        Page *page = bpm_->FetchPage(meta_pid);

        auto *meta =
            reinterpret_cast<IndexMetaEntryPage *>(page->GetData());

        bool match =
            (std::strncmp(meta->field_name,
                           field_name.c_str(),
                           sizeof(meta->field_name)) == 0);

        bpm_->UnpinPage(meta_pid, false);

        if (match) {
            return meta_pid;
        }
    }
    return INVALID_PAGE_ID;
}

} // namespace cmse

