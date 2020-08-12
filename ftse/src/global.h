#ifndef _FULL_TEXT_SEARCH_ENGINE_GLOBAL_H_ 
#define _FULL_TEXT_SEARCH_ENGINE_GLOBAL_H_
#include <vector>
#include <unordered_map>
#include <cstring>
namespace ftse {
#define UTF8_MAX_SIZE 4 // utf字符占最大字节数
#define N_GRAM 2 // 二元的Bi-Gram 
#define RET_FAIL 1
#define RET_SUCC 0

struct PostingsList {
    int document_id;
    std::vector<int> positions;
    // term frequence
    int positions_count;    // 文档内部这个token的数量
    PostingsList() {
        document_id = 0;
        positions.clear();
        positions_count = 0;
    }
/*
    PostingsList(PostingsList&& pl) {
        document_id = pl.document_id;
        positions_count = pl.positions_count;
        positions = std::move(pl.positions);
    }
*/
    PostingsList(int doc_id, int pos_count) {
        document_id = doc_id;
        positions.clear();
        positions_count = pos_count;
    }
};

struct InvertIndexEntry {
    int token_id;
    char token[2*UTF8_MAX_SIZE];
    // 按照document_id排序
    std::vector<PostingsList> postings_list;
    int positions_count;    // token在全部文档的数量
    // DF document frequence
    int documents_count;    // 出现过该词元的文档数
    InvertIndexEntry() {
        token_id = 0;
        postings_list.clear();
        positions_count = 0;
        documents_count = 0;
    }
    InvertIndexEntry(int token_id, int pos_count, int docs_count) {
        token_id = token_id;
        postings_list.clear();
        positions_count = pos_count;
        documents_count = docs_count;
    }
    InvertIndexEntry& operator=(const InvertIndexEntry& src) {
        if(this == &src) {
            return *this;
        }
        token_id = src.token_id;
        strncpy(token, src.token, 2*UTF8_MAX_SIZE);
        postings_list = src.postings_list;
        positions_count = src.positions_count;
        documents_count = src.documents_count;
        return *this;
    }
};
// utf32是固定长度编码占用4字节,32 bit
typedef uint32_t UTF32Char;
// key是token_id, value是InvertIndexEntry
typedef std::unordered_map<int, InvertIndexEntry> InvertIndex; 

typedef InvertIndex QueryToken;
typedef InvertIndexEntry QueryTokenEntry;
typedef PostingsList TokenPositionsList;

// 存储检索token的存储的倒排列表
struct DocSearchCursor {
  std::vector<TokenPositionsList> documents; /* 文档编号的序列 */
  std::vector<TokenPositionsList>::iterator current;   /* 当前的文档编号 */
};

struct PhraseSearchCursor {
  std::vector<int> positions; /* 位置信息 */
  int base;                  /* 词元在查询中的位置 */
  std::vector<int>::iterator current;              /* 当前的位置信息 */
};

//struct SearchResults{
//  int document_id;           /* 检索出的文档编号 */
//  double score;              /* 检索得分 */
//  UT_hash_handle hh;         /* 用于将该结构体转化为哈希表 */
//};
typedef std::unordered_map<int, double> SearchResults;
typedef std::pair<int, InvertIndexEntry> PAIR;
}
#endif // _FULL_TEXT_SEARCH_ENGINE_GLOBAL_H_
