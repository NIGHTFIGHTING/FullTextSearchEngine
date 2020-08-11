#include "buffer.h"
#include <cstring>
#include <stdlib.h>

using namespace ftse;
#define BUFFER_INIT_MIN 32 /* 分配缓冲区时的初始字节数 */
/**
 * 分配一个缓冲区
 * @return 指向分配好的缓冲区的指针
 */
Buffer *
Buffer::alloc_buffer(void)
{
    Buffer *buf;
    if ((buf = static_cast<Buffer*>(malloc(sizeof(Buffer)))))
    {
        if ((buf->head = static_cast<char*>(malloc(BUFFER_INIT_MIN))))
        {
            buf->curr = buf->head;
            buf->tail = buf->head + BUFFER_INIT_MIN;
            buf->bit = 0;
        }
        else
        {
            free(buf);
            buf = NULL;
        }
    }
    return buf;
}

/**
 * 扩大缓冲区的容量
 * @param[in,out] buf 指向待扩容的缓冲区的指针
 * @retval 0 成功
 * @retval 1 失败
 */
static int
enlarge_buffer(Buffer *buf)
{
    int new_size;
    char *new_head;
    new_size = (buf->tail - buf->head) * 2;
    if ((new_head = static_cast<char*>(realloc(buf->head, new_size))))
    {
        buf->curr = new_head + (buf->curr - buf->head);
        buf->tail = new_head + new_size;
        buf->head = new_head;
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * 将指定了字节数的数据添加到缓冲区中
 * @param[in] buf 指向要向里面添加数据的缓冲区的指针
 * @param[in] data 指向待添加的数据的指针
 * @param[in] data_size 待添加数据的字节数
 * @return 已添加至缓冲区中的数据的字节数
 */
int
Buffer::append_buffer(Buffer *buf, const void *data, unsigned int data_size)
{
    if (buf->bit)
    {
        buf->curr++;
        buf->bit = 0;
    }
    if (buf->curr + data_size > buf->tail)
    {
        if (enlarge_buffer(buf)) { return 0; }
    }
    if (data && data_size)
    {
        memcpy(buf->curr, data, data_size);
        buf->curr += data_size;
    }
    return data_size;
}

/**
 * 将1个比特的数据添加到缓冲区中
 * @param[in] buf 指向要向里面添加数据的缓冲区的指针
 * @param[in] bit 待添加的比特值。0或1
 */
void
Buffer::append_buffer_bit(Buffer *buf, int bit)
{
    if (buf->curr >= buf->tail)
    {
        if (enlarge_buffer(buf)) { return; }
    }
    if (!buf->bit) { *buf->curr = 0; }
    if (bit) { *buf->curr |= 1 << (7 - buf->bit); }
    if (++(buf->bit) == 8)
    {
        buf->curr++;
        buf->bit = 0;
    }
}

/**
 * 释放缓冲区
 * @param[in] buf 指向要释放的缓冲区的指针
 */
void
Buffer::free_buffer(Buffer *buf)
{
    free(buf->head);
    free(buf);
}
