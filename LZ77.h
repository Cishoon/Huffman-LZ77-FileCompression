#pragma once
typedef unsigned int uint;
typedef unsigned char uchar;
using namespace std;

#define WINDOW_SIZE_BIT 17
#define WINDOW_SIZE (1 << WINDOW_SIZE_BIT)

#define LENGTH_LIMIT_BIT 9
#define LENGTH_LIMIT (1 << LENGTH_LIMIT_BIT)

void LZ77_Compress(char* inFilePath, char* outFilePath);
void LZ77_unCompress(char* inFilePath, char* outFilePath);

uint gethash(int* ptr);
void longest_match(vector<uchar>& entire_text, uint& buf_begin, const uint& entire_size, uint& length, uint& distance, uint* hashtab);
void writebuf(stringbuf& com_str, uint& buf, uint& bufsize);
bool getbit(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize);
uchar getbyte(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize);
uint getbits(uchar*& ptr, uchar* end, uchar& bit, uchar& bitsize, int n);