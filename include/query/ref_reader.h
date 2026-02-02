#pragma once

#include <string>
#include "../common/types.h"


namespace cmse {

class RefReader {
public:
    explicit RefReader(const std::string &log_file);

    std::string Read(const RecordRef &ref);

private:
    int fd_;
};

} // namespace cmse

