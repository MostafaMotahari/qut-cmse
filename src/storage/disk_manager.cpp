#include "../../include/storage/disk_manager.h"

#include <cstring>   // for memset

namespace cmse {

DiskManager::DiskManager() {
    // Open file for read & write, binary mode
    file_.open(DISK_FILE_PATH,
               std::ios::in | std::ios::out | std::ios::binary);

    // If file does not exist, create it
    if (!file_.is_open()) {
        file_.open(DISK_FILE_PATH,
                   std::ios::out | std::ios::binary);
        file_.close();

        // Reopen in read/write mode
        file_.open(DISK_FILE_PATH,
                   std::ios::in | std::ios::out | std::ios::binary);
    }
}

DiskManager::~DiskManager() {
    file_.close();
}

void DiskManager::ReadPage(PageID page_id, char* data) {
    // Calculate where this page starts in the file
    long offset = page_id * PAGE_SIZE;

    // Move file cursor to that position
    file_.seekg(offset);

    // Try to read one full page
    file_.read(data, PAGE_SIZE);

    // If read failed (page does not exist yet)
    if (file_.gcount() < PAGE_SIZE) {
        std::memset(data, 0, PAGE_SIZE);
        file_.clear(); // clear error flags
    }
}

void DiskManager::WritePage(PageID page_id, const char* data) {
    long offset = page_id * PAGE_SIZE;

    file_.seekp(offset);
    file_.write(data, PAGE_SIZE);
    file_.flush();
}

} // namespace cmse

