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
	ifstream fin(inFilePath, ios::binary);  // 打开输入文件
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	int _start = clock();
	const size_t size = (size_t)fin.seekg(0, ios::end).tellg();
	if (size < 8 * 1024 * 10) {
		cerr << "文件过小，不支持压缩" << endl;
		fin.close();
		return;
	}
	/* 经过测试这里用vector读入速度最快，比原demo中快了将近200倍，stl大法好 */
	vector<unsigned char> entire_text(size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_text.size()));
	int _stop = clock();
	cout << "获取输入文件用时：" << _stop - _start << "ms" << endl;

	//创建哈夫曼树
	_start = clock();
	static HuffNode HuffTree[512];  //大小写死了，因为按字符压缩做多只用开512
	int root = buildHuffTree(HuffTree, entire_text, 256);
	_stop = clock();
	cout << "创建哈夫曼树用时：" << _stop - _start << "ms" << endl;

	_start = clock();
	//配置文件
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

	//转换成哈夫曼编码输出
	ofstream fout(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}
	fin.seekg(0, ios::beg);
	int ch = fin.get();
	unsigned char data = 0;  //压缩的数据以二进制的形式储存在文件中
	int pos = 7;
	string s = config_content;  //输出字符前置配置文件
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
	fout << pos + 1 << ' ';  //输出结尾多了几个没用的
	fout << s;
	fin.close();
	fout.close();

	_stop = clock();
	cout << "输出压缩文件用时：" << _stop - _start << "ms" << endl;

	cout << "压缩完成！" << endl;
	//cout << "压缩文件地址为：" << outFilePath << endl;
}

void huffmam_unCompress(char* comFilePath, char* uncomFilePath)
{
	ifstream fin(comFilePath, ios::binary);
	if (!fin.is_open()) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	int start = clock();
	//读取哈夫曼编码
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
	cout << "读入哈夫曼编码用时：" << stop - start << "ms" << endl;
	fin.clear();

	//读取压缩文件
	start = clock();
	int tmp = (int)fin.tellg() + 1;
	vector<unsigned char> content((const unsigned int)(fin.seekg(0, ios::end).tellg()) - tmp);
	fin.seekg(tmp, ios::beg).read((char*)&content[0], static_cast<std::streamsize>(content.size()));
	fin.close();
	stop = clock();
	cout << "读取压缩文件用时：" << stop - start << "ms" << endl;

	start = clock();
	ofstream fout(uncomFilePath, ios::binary);
	int now = root;
	string uncode;
	for (vector<unsigned char>::size_type i = 0; i < content.size(); i++) {
		int size_bit = 8;
		if (i == content.size() - 1)  //如果是最后一个字节
			size_bit -= surplus;

		for (int j = 1; j <= size_bit; j++) {
			if (content[i] & ((1 << (8 - j))))  //向右走
				now = HuffTree[now].rchild;
			else  //向左走
				now = HuffTree[now].lchild;

			if (HuffTree[now].lchild == -1)  //如果是叶子结点
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

	cout << "解压缩用时：" << stop - start << "ms" << endl;
}

void LZ77_Compress(char* inFilePath, char* outFilePath)
{
	ifstream fin(inFilePath, ios::binary);  // 打开输入文件
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	//获取输入文件
	int _start = clock();
	/* 经过测试这里用vector读入速度最快，比原demo中快了将近200倍，stl大法好 */
	const uint entire_size = (size_t)fin.seekg(0, ios::end).tellg();
	vector<uchar> entire_text(entire_size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_text.size()));
	fin.close();

	_start = clock();
	stringbuf com_str;  //储存压缩后的字符串

	ofstream fout;
	fout.open(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}

	uint buf = 0;      //缓冲区
	uint bufsize = 0;  //缓冲区字节数

	//初始化哈希表
	static uint hashtab[1 << 14 | 1];
	memset(hashtab, -1, sizeof(hashtab));

	//编码主体
	uint buf_begin;
	static int sum = 0;
	for (buf_begin = 0; buf_begin < entire_size;) {
		uint length = 0, distance = 0;
		longest_match(entire_text, buf_begin, entire_size, length, distance, hashtab);

		if (length == 0)  //如果不满足编码要求
		{
			buf = (buf << 9) | entire_text[buf_begin];  //开一个9bit空间，首位置0，表示未编码，后八位写入原字符
			bufsize += 9;
			writebuf(com_str, buf, bufsize);  //写入str

			//更新哈希表，把这个字符写进哈希表
			hashtab[gethash((int*)&entire_text[buf_begin])] = buf_begin;

			buf_begin++;  //缓冲区向前移动一位，滑动窗口向前移动一位
		}
		else  //如果满足编码要求
		{
			buf = (buf << (LENGTH_LIMIT_BIT + 1)) | (1 << LENGTH_LIMIT_BIT);  //开一个10bit空间，首位置1，表示编码，后面9个字节写入长度
			bufsize += LENGTH_LIMIT_BIT + 1;
			buf |= length;                                //写入长度
			writebuf(com_str, buf, bufsize);              //刷掉缓冲区
			buf = (buf << WINDOW_SIZE_BIT) | (distance);  //开15bit空间写入距离
			bufsize += WINDOW_SIZE_BIT;
			writebuf(com_str, buf, bufsize);  //刷掉缓冲区

			//更新哈希表，把buf_begin开始到buf_begin + length - 1的字符全部写入哈希表，同时移动了缓冲区
			for (uint i = 0; i < length; i++) {
				hashtab[gethash((int*)&entire_text[buf_begin])] = buf_begin;
				buf_begin++;
			}
		}
	}

	//如果缓冲区还有剩余，都刷掉
	if (bufsize != 0) {
		buf <<= (8 - bufsize);
		bufsize = 8;
		writebuf(com_str, buf, bufsize);
	}

	fout << com_str.str();


	fout.close();
	int _stop = clock();
	cout << "\n压缩成功！" << endl
	     << endl;
	cout << "总用时：" << _stop - _start << "ms";

	//获取大小
	fin.open(outFilePath, ios::binary);
	fin.clear();
	const size_t out_size = (size_t)fin.seekg(0, ios::end).tellg();
	fin.close();

	cout << "\t压缩率：" << setprecision(3) << 1.0 * out_size / entire_size * 100 << "%" << endl;
}



void LZ77_unCompress(char* inFilePath, char* outFilePath)
{
	int _start = clock();

	ifstream fin(inFilePath, ios::binary);  // 打开输入文件
	if (!fin) {
		cerr << "Can not open the input file!" << endl;
		return;
	}

	//获取输入文件
	/* 经过测试这里用vector读入速度最快，比原demo中快了将近200倍，stl大法好 */
	const size_t entire_size = (size_t)fin.seekg(0, ios::end).tellg();
	vector<uchar> entire_text(entire_size);
	fin.seekg(0, ios::beg).read((char*)&entire_text[0], static_cast<std::streamsize>(entire_size));
	fin.close();

	uchar* ptr = &entire_text[0];
	uchar* end = &entire_text[entire_size - 1];

	//解码
	char* unc_str = new (nothrow) char[1024 * 1024 * 10];
	if (unc_str == NULL) {
		cout << "申请内存失败！" << endl;
		exit(0);
	}
	char* pch = unc_str;

	uchar bit = 0;
	uchar bitsize = 0;
	uint now_byte = 0;  //现在是第几个字节
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
		else  //如果不是编码
		{
			uchar ch = getbyte(ptr, end, bit, bitsize);
			if (ch == '\0')
				break;
			*pch++ = ch;
		}
		if (ptr >= end)
			break;
	}

	//编码完成，全部存在com_str里面
	ofstream fout(outFilePath, ios::binary);
	if (!fout.is_open()) {
		cerr << "Can not open the output file!" << endl;
		return;
	}
	//*pch = 0;
	fout.write(unc_str,pch-unc_str);
	fout.close();
	cout << "\n解压成功！" << endl;
	int _stop = clock();
	cout << "\n总用时：" << _stop - _start << "ms" << endl;

	delete[] unc_str;
}
