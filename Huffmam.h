#pragma once
#include <climits>
#include <cstdio>
#include <cstring>
#include <string>
#include <queue>
#include <vector>
using namespace std;

#define INF INT_MAX
//���������ڵ�ṹ��
struct HuffNode
{
    int weight = 0;    //Ȩ��
    int parent = -1;   //���ڵ��±�
    int lchild = -1;   //�����±�
    int rchild = -1;   //�Һ����±�
    string bits = "";  //����������
};

int buildHuffTree(HuffNode HuffTree[], vector<unsigned char>& str, int n);

void huffman_Compress(char*, char*);
void huffmam_unCompress(char*, char*);