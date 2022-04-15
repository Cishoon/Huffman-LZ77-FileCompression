#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <time.h>
#include <sstream>
#include "Huffmam.h"
#include "LZ77.h"
using namespace std;

void huffman_Compress(char* inFilePath, char* outFilePath)
{
	ifstream fin(inFilePath, ios::binary);  // �������ļ�
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	int _start = clock();
	const size_t size = (size_t)fin.seekg(0, ios::end).tellg();
	if (size < 8 * 1024 * 10) {
		cerr << "�ļ���С����֧��ѹ��" << endl;
		fin.close();
		return;
	}
	/* ��������������vector�����ٶ���죬��ԭdemo�п��˽���200����stl�󷨺� */
	vector<unsigned char> entire_text(size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_text.size()));
	int _stop = clock();
	cout << "��ȡ�����ļ���ʱ��" << _stop - _start << "ms" << endl;

	//������������
	_start = clock();
	static HuffNode HuffTree[512];  //��Сд���ˣ���Ϊ���ַ�ѹ������ֻ�ÿ�512
	int root = buildHuffTree(HuffTree, entire_text, 256);
	_stop = clock();
	cout << "��������������ʱ��" << _stop - _start << "ms" << endl;

	_start = clock();
	//�����ļ�
	string config_content;
	int n = 256;
	for (int i = 0; i < n; i++) {
		if (HuffTree[i].bits.compare(""))
			config_content += HuffTree[i].bits + ' ';
		else
			config_content += "-1 ";
	}
	for (int i = 0; i < 2 * n - 1; i++) {
		config_content += to_string(HuffTree[i].parent) + ' ' + to_string(HuffTree[i].lchild) + ' ' + to_string(HuffTree[i].rchild) + ' ';
	}
	config_content += to_string(root) + ' ';

	//ת���ɹ������������
	ofstream fout(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}
	fin.seekg(0, ios::beg);
	int ch = fin.get();
	unsigned char data = 0;  //ѹ���������Զ����Ƶ���ʽ�������ļ���
	int pos = 7;
	string s = config_content;  //����ַ�ǰ�������ļ�
	while (ch != EOF) {
		const char* ptr = HuffTree[ch].bits.c_str();
		while (*ptr) {
			if (pos >= 0) {
				data = data | ((*ptr - '0') << pos);
				pos--;
			}
			if (pos < 0) {
				s += data;
				pos = 7;
				data = 0;
			}
			ptr++;
		}
		ch = fin.get();
	}
	if (data != 0)
		s += data;
	fout << pos + 1 << ' ';  //�����β���˼���û�õ�
	fout << s;
	fin.close();
	fout.close();

	_stop = clock();
	cout << "���ѹ���ļ���ʱ��" << _stop - _start << "ms" << endl;

	cout << "ѹ����ɣ�" << endl;
	//cout << "ѹ���ļ���ַΪ��" << outFilePath << endl;
}

void huffmam_unCompress(char* comFilePath, char* uncomFilePath)
{
	ifstream fin(comFilePath, ios::binary);
	if (!fin.is_open()) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	int start = clock();
	//��ȡ����������
	int surplus;
	fin >> surplus;
	static HuffNode HuffTree[512];
	for (int i = 0; i < 256; i++) {
		fin >> HuffTree[i].bits;
	}
	for (int i = 0; i < 256 * 2 - 1; i++) {
		fin >> HuffTree[i].parent >> HuffTree[i].lchild >> HuffTree[i].rchild;
	}
	int root = 0;
	fin >> root;
	int stop = clock();
	cout << "���������������ʱ��" << stop - start << "ms" << endl;
	fin.clear();

	//��ȡѹ���ļ�
	start = clock();
	int tmp = (int)fin.tellg() + 1;
	vector<unsigned char> content((const unsigned int)(fin.seekg(0, ios::end).tellg()) - tmp);
	fin.seekg(tmp, ios::beg).read((char*)&content[0], static_cast<std::streamsize>(content.size()));
	fin.close();
	stop = clock();
	cout << "��ȡѹ���ļ���ʱ��" << stop - start << "ms" << endl;

	start = clock();
	ofstream fout(uncomFilePath, ios::binary);
	int now = root;
	string uncode;
	for (vector<unsigned char>::size_type i = 0; i < content.size(); i++) {
		int size_bit = 8;
		if (i == content.size() - 1)  //��������һ���ֽ�
			size_bit -= surplus;

		for (int j = 1; j <= size_bit; j++) {
			if (content[i] & ((1 << (8 - j))))  //������
				now = HuffTree[now].rchild;
			else  //������
				now = HuffTree[now].lchild;

			if (HuffTree[now].lchild == -1)  //�����Ҷ�ӽ��
			{
				uncode += char(now);
				now = root;
			}
		}
	}
	fout << uncode;
	fout.close();
	fin.close();
	stop = clock();

	cout << "��ѹ����ʱ��" << stop - start << "ms" << endl;
}

void LZ77_Compress(char* inFilePath, char* outFilePath)
{
	ifstream fin(inFilePath, ios::binary);  // �������ļ�
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	//��ȡ�����ļ�
	int _start = clock();
	/* ��������������vector�����ٶ���죬��ԭdemo�п��˽���200����stl�󷨺� */
	const uint entire_size = (size_t)fin.seekg(0, ios::end).tellg();
	vector<uchar> entire_text(entire_size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_text.size()));
	fin.close();

	_start = clock();
	stringbuf com_str;  //����ѹ������ַ���

	ofstream fout;
	fout.open(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}

	uint buf = 0;      //������
	uint bufsize = 0;  //�������ֽ���

	//��ʼ����ϣ��
	static uint hashtab[1 << 14 | 1];
	memset(hashtab, -1, sizeof(hashtab));

	//��������
	uint buf_begin;
	static int sum = 0;
	for (buf_begin = 0; buf_begin < entire_size;) {
		uint length = 0, distance = 0;
		longest_match(entire_text, buf_begin, entire_size, length, distance, hashtab);

		if (length == 0)  //������������Ҫ��
		{
			buf = (buf << 9) | entire_text[buf_begin];  //��һ��9bit�ռ䣬��λ��0����ʾδ���룬���λд��ԭ�ַ�
			bufsize += 9;
			writebuf(com_str, buf, bufsize);  //д��str

			//���¹�ϣ��������ַ�д����ϣ��
			hashtab[gethash((int*)&entire_text[buf_begin])] = buf_begin;

			buf_begin++;  //��������ǰ�ƶ�һλ������������ǰ�ƶ�һλ
		}
		else  //����������Ҫ��
		{
			buf = (buf << (LENGTH_LIMIT_BIT + 1)) | (1 << LENGTH_LIMIT_BIT);  //��һ��10bit�ռ䣬��λ��1����ʾ���룬����9���ֽ�д�볤��
			bufsize += LENGTH_LIMIT_BIT + 1;
			buf |= length;                                //д�볤��
			writebuf(com_str, buf, bufsize);              //ˢ��������
			buf = (buf << WINDOW_SIZE_BIT) | (distance);  //��15bit�ռ�д�����
			bufsize += WINDOW_SIZE_BIT;
			writebuf(com_str, buf, bufsize);  //ˢ��������

			//���¹�ϣ����buf_begin��ʼ��buf_begin + length - 1���ַ�ȫ��д���ϣ��ͬʱ�ƶ��˻�����
			for (uint i = 0; i < length; i++) {
				hashtab[gethash((int*)&entire_text[buf_begin])] = buf_begin;
				buf_begin++;
			}
		}
	}

	//�������������ʣ�࣬��ˢ��
	if (bufsize != 0) {
		buf <<= (8 - bufsize);
		bufsize = 8;
		writebuf(com_str, buf, bufsize);
	}

	fout << com_str.str();


	fout.close();
	int _stop = clock();
	cout << "\nѹ���ɹ���" << endl
	     << endl;
	cout << "����ʱ��" << _stop - _start << "ms";

	//��ȡ��С
	fin.open(outFilePath, ios::binary);
	fin.clear();
	const size_t out_size = (size_t)fin.seekg(0, ios::end).tellg();
	fin.close();

	cout << "\tѹ���ʣ�" << setprecision(3) << 1.0 * out_size / entire_size * 100 << "%" << endl;
}



void LZ77_unCompress(char* inFilePath, char* outFilePath)
{
	int _start = clock();

	ifstream fin(inFilePath, ios::binary);  // �������ļ�
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	//��ȡ�����ļ�
	/* ��������������vector�����ٶ���죬��ԭdemo�п��˽���200����stl�󷨺� */
	const size_t entire_size = (size_t)fin.seekg(0, ios::end).tellg();
	vector<uchar> entire_text(entire_size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_size));
	fin.close();

	uchar* ptr = &entire_text[0];
	uchar* end = &entire_text[entire_size - 1];

	//����
	char* unc_str = new (nothrow) char[1024 * 1024 * 10];
	if (unc_str == NULL) {
		cout << "�����ڴ�ʧ�ܣ�" << endl;
		exit(0);
	}
	char* pch = unc_str;

	uchar bit = 0;
	uchar bitsize = 0;
	uint now_byte = 0;  //�����ǵڼ����ֽ�
	while (1) {
		bool iscode = getbit(ptr, end, bit, bitsize);
		if (iscode) {
			uint length = getbits(ptr, end, bit, bitsize, LENGTH_LIMIT_BIT);
			uint distance = getbits(ptr, end, bit, bitsize, WINDOW_SIZE_BIT);

			char* tmp = pch - distance;
			for (uint i = 0; i < length; i++) {
				*pch++ = *tmp++;
			}
		}
		else  //������Ǳ���
		{
			uchar ch = getbyte(ptr, end, bit, bitsize);
			if (ch == '\0')
				break;
			*pch++ = ch;
		}
		if (ptr >= end)
			break;
	}

	//������ɣ�ȫ������com_str����
	ofstream fout(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}
	//*pch = 0;
	fout.write(unc_str,pch-unc_str);
	fout.close();
	cout << "\n��ѹ�ɹ���" << endl;
	int _stop = clock();
	cout << "\n����ʱ��" << _stop - _start << "ms" << endl;

	delete[] unc_str;
}
