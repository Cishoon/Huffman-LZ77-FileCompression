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



//��ȡ��ϣֵ
//��ǰ�ĸ��ַ�����
uint gethash(int* ptr)
{
	static const uint fib = 92083;		//5.81
	return (*ptr * fib) >> 18;
}


/***************************************************************************
  ��    �ܣ����ֵ����У��ҵ������ͬ�ַ�������ó��Ⱥ;���
  ���������
            entire_text     :       �����ַ���
            buf_begin       :       ��������ʼ��λ��
            entire_size     :       �����ַ����ĳ���(��ʡ����.size()��ʱ��)
            length          :       ��ͬ�ַ��������
            distance        :       ������ͬ�ַ��������ַ��ľ���
            hashtab         :       ��ϣ�������Ż��㷨
***************************************************************************/
void longest_match(vector<uchar>& entire_text, uint& buf_begin, const uint& entire_size, uint& length, uint& distance, uint* hashtab)
{
	uint dic_begin = hashtab[gethash((int*)&entire_text[buf_begin])];  //��ȡ�ֵ�������ͬ�ַ������׵�ַ
	if (dic_begin == -1)                                               //����ֵ����Ҳ��������������������ַ���
	{
		length = 0;
		distance = 0;
		return;
	}

	distance = buf_begin - dic_begin;
	if (distance > WINDOW_SIZE)  //�������̫���ˣ��͵���û�ҵ�
	{
		length = 0;
		distance = 0;
		return;
	}

	//���㳤��
	char *p1 = (char*)&entire_text[dic_begin], *p2 = (char*)&entire_text[buf_begin];
	while (*p1++ == *p2++)
		length++;

	if (length >= LENGTH_LIMIT || length <= 4)  //�������̫����Ҳ��д��
	{
		length = 0;
		distance = 0;
	}
}

//д�뻺����������ݵ��ַ���
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
	return (bit >> bitsize) & 1;  //ÿ�η����ֽڵ�һλ
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
