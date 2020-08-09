#ifndef _FULL_TEXT_SEARCH_ENGINE_POSTINGS_H 
#define _FULL_TEXT_SEARCH_ENGINE_POSTINGS_H
#include "global.h"
#include <vector>
namespace ftse {
class FullTextSearchEngineEnv;
class Buffer;
class Postings {
public:
    Postings(); 
    static void encode_postings(
            const std::vector<PostingsList>& postings, Buffer* postings_e);
    static void decode_postings(
            const char* postings_extend, int postings_extend_size,
            std::vector<PostingsList>* postings, int* decode_len);
    static int fetch_postings(FullTextSearchEngineEnv& ftse_env,
            int token_id, std::vector<PostingsList>* postings);
    static void update_postings(FullTextSearchEngineEnv& ftse_env,
            InvertIndexEntry& p);
    static std::vector<PostingsList> merge_postings(std::vector<PostingsList>& pa,
            std::vector<PostingsList>& pb);
    static void merge_invert_index(InvertIndex& base_ii, InvertIndex& to_be_added_ii);
};
}
#endif // _FULL_TEXT_SEARCH_ENGINE_POSTINGS_H
