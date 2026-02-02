#pragma once

#include <string>
#include <cstdint>

namespace cmse {

enum class QueryOp {
    EQUALS,
    BETWEEN,
    STARTSWITH
};

struct Query {
    std::string field_name;
    QueryOp op;

    uint64_t num_value;      // for EQUALS
    uint64_t low, high;      // for BETWEEN
    std::string str_value;   // for string ops
};

} // namespace cmse

