#pragma once
#include <climits>
#include <cstdio>
#include <cstring>
#include <string>
#include <queue>
#include <vector>
using namespace std;

#define INF INT_MAX
//哈夫曼树节点结构体
struct HuffNode
{
    int weight = 0;    //权重
    int parent = -1;   //父节点下标
    int lchild = -1;   //左孩子下标
    int rchild = -1;   //右孩子下标
    string bits = "";  //哈夫曼编码
};

int buildHuffTree(HuffNode HuffTree[], vector<unsigned char>& str, int n);

void huffman_Compress(char*, char*);
void huffmam_unCompress(char*, char*);