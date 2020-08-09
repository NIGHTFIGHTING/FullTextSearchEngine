#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

typedef uint32_t
        UTF32Char; /* 经过UTF-32编码的Unicode字符串 */
#define MAX_UTF8_SIZE 4 /* 用UTF-8表示1个Unicode字符最多需要多少个字节 */


namespace ftse {
class Encoding {
public:
    static int print_error(const char *format, ...);

    static int uchar2utf8_size(const UTF32Char *ustr, int ustr_len);

    static char *utf32toutf8(const UTF32Char *ustr, int ustr_len, char *str,
            int *str_size);

    static int utf8toutf32(const char *str, int str_size, UTF32Char **ustr,
            int *ustr_len);

    static void print_time_diff(void);
};
}

#endif /* __UTIL_H__ */
