#include "../../include/query/query_parser.h"
#include <sstream>

namespace cmse {

bool QueryParser::Parse(const std::string &q, Query &out) {
    std::istringstream ss(q);
    std::string where, field, op;

    ss >> where >> field >> op;

    if (where != "WHERE") return false;

    out.field_name = field;

    if (op == "EQUALS") {
        std::string value;
        ss >> value;

        if (value.front() == '"') {
            // string equals
            out.op = QueryOp::EQUALS;
            out.str_value = value.substr(1, value.size() - 2);
        } else {
            // numeric equals
            out.op = QueryOp::EQUALS;
            out.num_value = std::stoull(value);
        }
        return true;
    }

    if (op == "BETWEEN") {
        ss >> out.low;
        ss.ignore(1); // comma
        ss >> out.high;
        out.op = QueryOp::BETWEEN;
        return true;
    }

    if (op == "STARTSWITH") {
        std::string value;
        ss >> value;
        out.op = QueryOp::STARTSWITH;
        out.str_value = value.substr(1, value.size() - 2);
        return true;
    }

    return false;
}

} // namespace cmse

