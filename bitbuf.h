#ifndef __BITBUF_H
#define __BITBUF_H

/*
 * 实现一个bitbuf类型的缓冲器数据结构
 * -不断向里面写入0或1字符，缓冲器满则输出一个long类型的数字，
 *  并清空缓冲器。
 * -或者先给缓存器赋值，按位返回0或1字符。
 *
 * 举例：依次输入的字符序列为
 * 0101 0101 1111 10
 * 输出的数字为
 * high                                     low
 * 63         55         47                 0
 *  ---- ----  ---- ----  ---- .... .... ----
 * |0101 0101  1111 1000  0000           0000|
 *  -----------------------------------------
 *  current_len = 14
 */

#define buf_length	(sizeof(long) * 8)

struct bitbuf {
	unsigned long value;
	unsigned int current_len;
};

int is_buf_filled(struct bitbuf *buf)
{
	return buf->current_len == buf_length;
}

int is_buf_empty(struct bitbuf *buf)
{
	return buf->current_len == 0;
}

unsigned int buf_get_length(struct bitbuf *buf)
{
	return buf->current_len;
}

void buf_set_length(unsigned int length, struct bitbuf *buf)
{
	buf->current_len = length;
}

unsigned long buf_get_value(struct bitbuf *buf)
{
	return buf->value;
}

void buf_set_value(unsigned long val, struct bitbuf *buf)
{
	buf->value = val;
	buf->current_len = buf_length;
}

void buf_clean(struct bitbuf *buf)
{
	buf->value = 0;
	buf->current_len = 0;
}

//假设输入是合法的，buf非满
void buf_putchar(char ch, struct bitbuf *buf)
{
	if (ch == '1')
		buf->value |= 1UL << (buf_length - 1 - buf->current_len);
	buf->current_len++;
}

//假设输入是合法的，buf非空
char buf_getchar(struct bitbuf *buf)
{
	char retval = '0' + (buf->value >> (buf_length -1));
	buf->current_len--;
	buf->value <<= 1;
	return retval;
}

#endif
