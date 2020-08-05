#ifndef FULL_TEXT_SEARCH_ENGINE_POSTINGS_H 
#define FULL_TEXT_SEARCH_ENGINE_POSTINGS_H
#include "global.h"
#include <vector>
namespace ftse {
class Postings {
public:
    Postings(); 
    static std::vector<PostingsList> merge_postings(std::vector<PostingsList>& pa,
            std::vector<PostingsList>& pb);
    static void merge_invert_index(InvertIndex& base_ii, InvertIndex& to_be_added_ii);
};
}
#endif // FULL_TEXT_SEARCH_ENGINE_POSTINGS_H
