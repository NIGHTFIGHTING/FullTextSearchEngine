#include "token.h"
#include "postings.h"
#include "ftse_env.h"
#include "encoding.h"
#include "utils.h"
#include <iostream>
#include <cstring>
using namespace std;

using namespace ftse;
Token::Token() {
}
int Token::is_ignored_char(const UTF32Char ustr) {
    switch (ustr) {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
        case '!':
        case '"':
        case '#':
        case '$':
        case '%':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case '-':
        case '.':
        case '/':
        case ':':
        case ';':
        case '<':
        case '=':
        case '>':
        case '?':
        case '@':
        case '[':
        case '\\':
        case ']':
        case '^':
        case '_':
        case '`':
        case '{':
        case '|':
        case '}':
        case '~':
        case 0x3000: /* 全角空格 */
        case 0x3001: /* 、 */
        case 0x3002: /* 。 */
        case 0xFF08: /* （ */
        case 0xFF09: /* ） */
        case 0xFF01: /* ！ */
        case 0xFF0C: /* ， */
        case 0xFF1A: /* ： */
        case 0xFF1B: /* ； */
        case 0xFF1F: /* ? */
            return 1;
        default:
            return 0;
    }
}
/**
 *
 * start标记当前ngram的开始
 **/
int Token::ngram_next(const UTF32Char *ustr, const UTF32Char *ustr_end,
           unsigned int token_len, const UTF32Char **start) {
    for(; ustr < ustr_end; ++ustr) {
        if(!is_ignored_char(*ustr)) {
            break;
        }
    }
    const UTF32Char* ite = ustr;
    for(unsigned int i = 0; i < token_len && ite < ustr_end; ++i, ++ite) {
        if(is_ignored_char(*ite)) {
            break;
        }
    }
    *start = ustr;
    // 返回ngram片段长度
    return ite-ustr; 
}
void Token::text_to_postings_lists(FullTextSearchEngineEnv& ftse_env,
        const int document_id,
        const UTF32Char *text, const unsigned int text_len,
        const int token_len, InvertIndex* postings) {
    if(!text || text_len <=0) {
        return;
    }
    // 当前字符串的末尾
    const UTF32Char* text_end = text + text_len;
    // 倒排列表位置
    int position = 0;
    // 切割的ngram的片段长度
    int t_len;
    // buffer_index表示对这个doc建立的临时倒排索引
    InvertIndex buffer_index;
    // ite表示每次都移动一个字
    // ngram_next每次都提取token_len的片段
    for(const UTF32Char* ite = text;
            (t_len = ngram_next(ite, text_end, token_len, &ite)); ++ite, ++position) {
        // 创建倒排时候，小于N-gram长度的token，也存储在索引库中
        // 检索时候，忽略掉由t中长度不足N-gram的最后几个字符构成的词元
        if(t_len >= token_len || document_id) {
            cout << "position: " << position << " t_len: " << t_len << endl; 
            int token_utf8_size;
            // t_len是截取结果字符个数 
            char token_utf8[t_len*UTF8_MAX_SIZE]; 
            // 将切好词的token转为utf8编码
            Encoding::utf32toutf8(ite, t_len, token_utf8, &token_utf8_size);
            cout << "token_utf8: " << token_utf8 << endl;
            token_to_postings_lists(ftse_env, document_id, token_utf8,
                    token_utf8_size,
                    position,
                    &buffer_index);
        }
    }
    Utils::print_invert_index_info(buffer_index);
    // 将临时倒排索引merge到累积倒排索引
    if(buffer_index.size() > 0 && postings) {
        Postings::merge_invert_index(*postings,
                buffer_index);
    }
}
void Token::token_to_postings_lists(FullTextSearchEngineEnv& ftse_env,
        const int document_id,
        const char *token,
        const unsigned int token_size,
        const int position,
        InvertIndex* buffer_index) {
    int documents_count = 0; 
    int token_id = ftse_env.db().db_get_token_id(token, token_size, document_id, &documents_count);
    
    if(buffer_index->find(token_id) != buffer_index->end()) {
        InvertIndexEntry& ii_entry = (*buffer_index)[token_id];
        if(ii_entry.postings_list.size() >= 1) {
            PostingsList& pl = ii_entry.postings_list[0];
            ++pl.positions_count;
            pl.positions.push_back(position);
        }
        ++ii_entry.positions_count;
        //strncpy(ii_entry.token, token, token_size);
    } else {
        // 1:token在所有doc中的数量
        // 1:token在在几个dic中出现
        // document_id为0的情况是search时候用的,为了计算TF-IDF
        InvertIndexEntry ii_entry(token_id, 1,
            document_id != 0 ? 1 : documents_count);
        // 出现的doc，以及在这个doc出现的次数
        PostingsList pl(document_id, 1);
        pl.positions.push_back(position);
        // 这个token出现的doc
        ii_entry.postings_list.push_back(std::move(pl));
        strncpy(ii_entry.token, token, N_GRAM*UTF8_MAX_SIZE);
        (*buffer_index)[token_id] = ii_entry; 
    }
    Utils::print_invert_index_info(*buffer_index, token_id);
    // 当前token在文档中出现的总次数
}
