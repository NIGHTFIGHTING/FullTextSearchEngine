#ifndef _FULL_TEXT_SEARCH_ENGINE_BUFFER_H_ 
#define _FULL_TEXT_SEARCH_ENGINE_BUFFER_H_

namespace ftse {
class Buffer {
//private:
public:
    char *head;       /* 指向缓冲区的开头 */
    char *curr;       /* 指向缓冲区中的当前位置 */
    const char *tail; /* 指向缓冲区的结尾 */
    int bit;          /* 缓冲区的当前位置（以比特为单位） */
public:
    static Buffer *alloc_buffer(void);

    static int append_buffer(Buffer *buf, const void *data,
            unsigned int data_size);

    static void free_buffer(Buffer *buf);

    static void append_buffer_bit(Buffer *buf, int bit);
};
#define BUFFER_PTR(b) ((b)->head) /* 返回指向缓冲区开头的指针 */
#define BUFFER_SIZE(b) ((b)->curr - (b)->head) /* 返回缓冲区的大小 */
}
#endif // _FULL_TEXT_SEARCH_ENGINE_BUFFER_H_
