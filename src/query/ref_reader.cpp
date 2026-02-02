#include "../../include/query/ref_reader.h"
#include <unistd.h>
#include <fcntl.h>

namespace cmse {

RefReader::RefReader(const std::string &file) {
    fd_ = open(file.c_str(), O_RDONLY);
}

std::string RefReader::Read(const RecordRef &ref) {
    char buffer[1024];
    lseek(fd_, ref.offset, SEEK_SET);
    ssize_t n = read(fd_, buffer, sizeof(buffer));
    return std::string(buffer, n);
}

} // namespace cmse

