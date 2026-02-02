#pragma once

#include "../index/index_catalog.h"
#include "../index/btree/bplus_tree.h"
#include "../index/trie/trie.h"
#include "query_types.h"
#include "ref_reader.h"

namespace cmse {

class QueryExecutor {
public:
    QueryExecutor(BufferPoolManager *bpm, IndexCatalog *catalog, RefReader *reader);

    void Execute(const Query &query);

private:
    BufferPoolManager *bpm_;
    IndexCatalog *catalog_;
    RefReader *reader_;
};

} // namespace cmse

