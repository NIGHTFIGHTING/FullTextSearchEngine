#ifndef FULL_TEXT_SEARCH_ENGINE_TOKEN_H 
#define FULL_TEXT_SEARCH_ENGINE_TOKEN_H
#include "global.h"
namespace ftse {
class FullTextSearchEngineEnv;
class Token {
public:
    Token(); 
    // ftse_env获得db, postings是通过n-gram获得的倒排列表
    static void text_to_postings_lists(FullTextSearchEngineEnv& ftse_env,
            const int document_id,
            const UTF32Char *text, const unsigned int text_len,
            const int token_len,
            InvertIndex* postings);
    static int is_ignored_char(const UTF32Char ustr);
    static int ngram_next(const UTF32Char *ustr, const UTF32Char *ustr_end,
            unsigned int token_len, const UTF32Char **start);
    static void token_to_postings_lists(FullTextSearchEngineEnv& ftse_env,
            const int document_id,
            const char *token,
            const unsigned int token_size,
            const int position,
            InvertIndex* buffer_index);
};
}
#endif // FULL_TEXT_SEARCH_ENGINE_TOKEN_H
