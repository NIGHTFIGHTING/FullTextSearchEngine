#ifndef _FULL_TEXT_SEARCH_ENGINE_UTILS_H_
#define _FULL_TEXT_SEARCH_ENGINE_UTILS_H_
#include "global.h"
namespace ftse{
class Utils {
public:
    static void print_invert_index_info(InvertIndex& index, int token_id);
    static void print_invert_index_info(InvertIndex& index);
};
}
#endif // _FULL_TEXT_SEARCH_ENGINE_UTILS_H_
