#include "search.h"
#include "encoding.h"
#include "token.h"
#include "ftse_env.h"
#include "postings.h"
#include <vector>
#include <algorithm>

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

int Search::search_phrase(const std::vector<PAIR>& query_tokens,
        const std::vector<DocSearchCursor>& doc_cursors) {
    int n_positions = 0;
    for(auto& ite : query_tokens) {
        n_positions += ite.second.positions_count;
    }
    std::vector<PhraseSearchCursor> cursors(n_positions);
    int cur = 0;
    for(auto& ite : query_tokens) {
        // 因为对于query_token只存在一个doc，所以直接取得ostings_list[0]
        for(int i = 0; i < ite.second.postings_list[0].positions.size(); ++i) {
            cursors[cur].base = ite.second.postings_list[0].positions[i];
            cursors[cur].positions = doc_cursors[i].current->positions;
            cursors[cur].current = cursors[cur].positions.begin();
            ++cur;
        }
    }
    int phrase_count = 0;
    while(cursors[0].current != cursors[0].positions.end()) {
        int rel_position, next_rel_position;
        rel_position = next_rel_position = *(cursors[0].current) - cursors[0].base;
        for(int i = 1; i < cursors.size(); ++i) {
            while(cursors[i].current != cursors[i].positions.end()
                    && *(cursors[i].current) - cursors[cur].base < rel_position) {
                ++cursors[i].current;
            }
            if(cursors[i].current == cursors[i].positions.end()) {
                return phrase_count;
            }
            if(*(cursors[i].current) - cursors[cur].base != rel_position) {
                next_rel_position = *(cursors[i].current) - cursors[cur].base;
                break;
            }
        }
        if(next_rel_position > rel_position) {
            while(cursors[0].current != cursors[0].positions.end()
                    && *(cursors[0].current) - cursors[0].base < next_rel_position) {
                ++cursors[0].current;
            }
        } else {
            ++phrase_count;
            ++cursors[0].current;
        }
    }
    return phrase_count;
}

void Search::search_docs(FullTextSearchEngineEnv& ftse_env, const QueryToken& query_tokens,
        SearchResults* results) {
    std::vector<PAIR> tokens(
            query_tokens.begin(), query_tokens.end());
    // 按照token在所有doc中出现的次数排序
    sort(tokens.begin(), tokens.end(),[](const PAIR& a, const PAIR& b)
            { return a.second.documents_count < b.second.documents_count; });
    std::vector<DocSearchCursor> cursors(tokens.size());
    for(int i = 0; i < tokens.size(); ++i) {
        /* 当前的token在构建索引的过程中从未出现过 */
        if(tokens[i].second.token_id == 0) {
            return;
        }
        Postings::fetch_postings(ftse_env, tokens[i].second.token_id, &cursors[i].documents);
        /* 虽然当前的token存在，但是由于更新或删除导致其倒排列表为空 */
        if(cursors[i].documents.size() == 0) {
            return;
        }
        cursors[i].current = cursors[i].documents.begin();
    }
    while(cursors[0].current != cursors[0].documents.end()) {
        /* 将拥有文档最少的词元称作A */
        int doc_id = cursors[0].current->document_id;
        int next_doc_id = 0;
        for(int j = 1; j < cursors.size(); ++j) {
            while(cursors[j].current != cursors[j].documents.end()
                    && cursors[j].current->document_id < doc_id) {
                cursors[j].current++;
            } 
            // token没有在同一个doc_id中
            if(cursors[j].current == cursors[j].documents.end()) {
                return;
            }
            // 标记下一个可能的doc_id 
            if(cursors[j].current->document_id != doc_id) {
                next_doc_id = cursors[j].current->document_id;
                break;
            }
        }
        if(next_doc_id > 0) {
            /* 不断获取A的下一个document_id，直到其当前的document_id不小于next_doc_id为止 */
            while(cursors[0].current != cursors[0].documents.end()
                    && cursors[0].current->document_id < next_doc_id) {
                cursors[0].current++;
            }
        } else {
            int phrase_count = -1;
            if (ftse_env.get_enable_phrase_search()) {
                phrase_count = search_phrase(tokens, cursors);
            } else {
            }
        }
    }
}

void Search::search(FullTextSearchEngineEnv& ftse_env, const char* query) {
    int query32_len;
    UTF32Char* query32; 
    if (RET_SUCC == Encoding::utf8toutf32(query, strlen(query), &query32, &query32_len)) {
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
