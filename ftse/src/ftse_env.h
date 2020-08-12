#ifndef _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_ 
#define _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_
#include <unordered_map>
#include "global.h"
#include "database.h"
namespace ftse {
class FullTextSearchEngineEnv {
public:
    FullTextSearchEngineEnv(int token_len, int threshold, bool enable_phrase_search);
    Database& db();
    InvertIndex& get_invert_index();
    void add_document(const char *title, const char *body);
    int get_token_len();
    bool get_enable_phrase_search();
private:
    InvertIndex invert_index_; 
    Database db_;
    int token_len_;
    int ii_update_threshold_;
    bool enable_phrase_search_;
};
}
#endif // _FULL_TEXT_SEARCH_ENGINE_FTSE_ENV_H_
