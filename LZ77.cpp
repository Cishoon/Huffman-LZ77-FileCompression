#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "LZ77.h"
using namespace std;



//获取哈希值
//用前四个字符编码
uint gethash(int* ptr)
{
	static const uint fib = 92083;		//5.81
	return (*ptr * fib) >> 18;
}


/***************************************************************************
  功    能：在字典区中，找到最长的相同字符串，获得长度和距离
  输入参数：
            entire_text     :       整个字符串
            buf_begin       :       缓冲区开始的位置
            entire_size     :       整个字符串的长度(节省调用.size()的时间)
            length          :       相同字符串最长长度
            distance        :       距离相同字符串的首字符的距离
            hashtab         :       哈希表，用于优化算法
***************************************************************************/
void longest_match(vector<uchar>& entire_text, uint& buf_begin, const uint& entire_size, uint& length, uint& distance, uint* hashtab)
{
	uint dic_begin = hashtab[gethash((int*)&entire_text[buf_begin])];  //获取字典序中相同字符串的首地址
	if (dic_begin == -1)                                               //如果字典中找不到三个及以上连续的字符串
	{
		length = 0;
		distance = 0;
		return;
	}

	distance = buf_begin - dic_begin;
	if (distance > WINDOW_SIZE)  //如果距离太大了，就当做没找到
	{
		length = 0;
		distance = 0;
		return;
	}

	//计算长度
	char *p1 = (char*)&entire_text[dic_begin], *p2 = (char*)&entire_text[buf_begin];
	while (*p1++ == *p2++)
		length++;

	if (length >= LENGTH_LIMIT || length <= 4)  //如果距离太长，也不写入
	{
		length = 0;
		distance = 0;
	}
}

//写入缓冲区里的内容到字符串
void writebuf(stringbuf& com_str, uint& buf, uint& bufsize)
{
	while (bufsize >= 8) {
		com_str.sputc((buf >> (bufsize - 8)));
		bufsize -= 8;
	}
}


bool getbit(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize)
{
	if (bitsize == 0) {
		bit = *ptr++;
		bitsize = 8;
	}
	bitsize--;
	return (bit >> bitsize) & 1;  //每次返回字节的一位
}

uchar getbyte(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize)
{
	if (bitsize == 0) {
		bit = *ptr++;
		bitsize = 8;
	}

	uchar val = (bit << (8 - bitsize));
	bit = *ptr++;
	val |= (bit >> bitsize);
	return val;
}

uint getbits(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize, int n)
{
	uint val = 0;
	while (n >= 8) {
		val = (val << 8) | getbyte(ptr, end, bit, bitsize);
		n -= 8;
	}
	while (n--) {
		val = (val << 1) + getbit(ptr, end, bit, bitsize);
	}
	return val;
}
