#include "search.h"
#include "encoding.h"
#include "token.h"
#include "ftse_env.h"

using namespace ftse;


void Search::split_query_to_tokens(FullTextSearchEngineEnv& ftse_env,
        const UTF32Char *text,
        const unsigned int text_len,
        const int n, QueryToken* query_tokens) {
    return Token::text_to_postings_lists(ftse_env,
            0, /* 将document_id设为0 */
            text, text_len, n,
            query_tokens);
}

void Search::search_docs(FullTextSearchEngineEnv& ftse_env, const QueryToken& query_tokens,
        SearchResults* results) {
}

void Search::search(FullTextSearchEngineEnv& ftse_env, const char* query) {
    int query32_len;
    UTF32Char* query32; 
    if (!Encoding::utf8toutf32(query, strlen(query), &query32, &query32_len)) {
        SearchResults results;
        if (query32_len < ftse_env.get_token_len()) {
            Encoding::print_error("too short query.");
        } else { 
            QueryToken query_tokens;
            split_query_to_tokens(ftse_env, query32, query32_len, ftse_env.get_token_len(), &query_tokens);
            search_docs(ftse_env, query_tokens, &results);
        }
    }
}
