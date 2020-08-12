#ifndef _FULL_TEXT_SEARCH_ENGINE_SEARCH_H_ 
#define _FULL_TEXT_SEARCH_ENGINE_SEARCH_H_

#include "global.h"
namespace ftse {
class FullTextSearchEngineEnv;
class Search {
public:
    void search(FullTextSearchEngineEnv& ftse_env, const char* query);
    int search_phrase(const std::vector<PAIR>& query_tokens,
            const std::vector<DocSearchCursor>& doc_cursors);
    void search_docs(FullTextSearchEngineEnv& ftse_env, const QueryToken& query_tokens,
            SearchResults* results);
    void split_query_to_tokens(FullTextSearchEngineEnv& ftse_env,
            const UTF32Char *text,
            const unsigned int text_len,
            const int n, QueryToken* query_tokens);
};
}
#endif // _FULL_TEXT_SEARCH_ENGINE_SEARCH_H_
