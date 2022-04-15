#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Huffmam.h"
#include "LZ77.h"
using namespace std;

int main(int argc, char* argv[])
{
	cout << "Zipper 2.00! Author: Cishoon" << endl;  //作业编号

	//判断是否输入正确//
	if (argc != 4) {
		cerr << "Please make sure the number of parameters is correct." << endl;
		return -1;
	}

	if (strcmp(argv[3], "zip") && strcmp(argv[3], "unzip")) {
		cerr << "Unknown parameter!\nCommand list:\nzip, unzip" << endl;
		return -1;
	}

	//ver1.0 纯粹的huffman编码
	//if (!strcmp(argv[3], "zip"))
	//    huffman_Compress(argv[1], argv[2]);
	//else if (!strcmp(argv[3], "unzip"))
	//    huffmam_unCompress(argv[1], argv[2]);

	//ver2.0 LZ77
	if (!strcmp(argv[3], "zip"))
		LZ77_Compress(argv[1], argv[2]);
	else if (!strcmp(argv[3], "unzip"))
		LZ77_unCompress(argv[1], argv[2]);


	return 0;
}