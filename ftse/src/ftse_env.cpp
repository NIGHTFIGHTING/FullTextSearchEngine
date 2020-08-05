#include "ftse_env.h"

using namespace ftse;
FullTextSearchEngineEnv::FullTextSearchEngineEnv():db_("./ftse.db") {
}

Database& FullTextSearchEngineEnv::db() {
    return db_;
}
InvertIndex& FullTextSearchEngineEnv::get_invert_index() {
    return invert_index_;
}
