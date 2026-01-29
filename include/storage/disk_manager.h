#pragma once

#include <fstream>
#include <string>

#include "../common/types.h"
#include "../common/constants.h"
#include "../common/config.h"

namespace cmse {

class DiskManager {
public:
    DiskManager();
    ~DiskManager();

    void ReadPage(PageID page_id, char* data);
    void WritePage(PageID page_id, const char* data);

private:
    std::fstream file_;
};

} // namespace cmse

