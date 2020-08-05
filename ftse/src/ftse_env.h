#ifndef _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_ 
#define _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_
#include <unordered_map>
#include "global.h"
#include "database.h"
namespace ftse {
class FullTextSearchEngineEnv {
public:
    FullTextSearchEngineEnv();
    Database& db();
    InvertIndex& get_invert_index();
private:
    InvertIndex invert_index_; 
    Database db_;
};
}
#endif // _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_
