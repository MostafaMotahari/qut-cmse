#pragma once

#include "query_types.h"
#include <string>

namespace cmse {

class QueryParser {
public:
    static bool Parse(const std::string &query_str, Query &out);
};

} // namespace cmse

