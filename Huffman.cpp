#define _CRT_SECURE_NO_WARNINGS
#include <queue>
#include <iostream>
#include "Huffmam.h"
using namespace std;


/***************************************************************************
  ��    �ܣ�ѡ��Ȩ����С����������������
  ���������
            lc      :       Ȩ����С�Ľڵ��±�
            rc      :       Ȩ�صڶ�С�Ľڵ��±�
            n       :       ���������ڵ�����
  ˵    ��������ö�٣�
            ����ʹ�����ȶ����Ż����������Ч�ʻ�����ֱ�ӱ�������֪����ʲôԭ��
***************************************************************************/
void selectHuffmanNode(HuffNode* HF, int& lc, int& rc, int n)
{
    int min1 = INF;
    int min2 = INF;
    for (int i = 0; i < 2 * n - 1; i++) {
        if (HF[i].weight != 0 && HF[i].parent == -1) {
            if (HF[i].weight < min1) {
                min2 = min1;
                rc = lc;
                min1 = HF[i].weight;
                lc = i;
            }
            else if (HF[i].weight < min2) {
                min2 = HF[i].weight;
                rc = i;
            }
        }
    }
}

/***************************************************************************
  ��    �ܣ����ù���������
  ���������
            HuffTree        :       �������������׵�ַ
            p               :       �±�
            bits            :       ����������
  ˵    ��������
***************************************************************************/
void setHuffmanCode(HuffNode* HuffTree, int p, string bits)
{
    if (HuffTree[p].lchild == -1 && HuffTree[p].rchild == -1)  //����������Ҷ�ӽ��
    {
        HuffTree[p].bits = bits;
        return;
    }
    setHuffmanCode(HuffTree, HuffTree[p].lchild, bits + "0");
    setHuffmanCode(HuffTree, HuffTree[p].rchild, bits + "1");
}

/***************************************************************************
  ��    �ܣ�������������
  ���������
            HuffTree        :       �������������׵�ַ
            str             :       �����ļ��������ݵ�vector����
            n               :       �ڵ�����
  �� �� ֵ�����ڵ��±�
***************************************************************************/
int buildHuffTree(HuffNode HuffTree[], vector<unsigned char>& str, int n)
{
    //��ȡÿ���ڵ��Ȩ��
    for (vector<char>::size_type i = 0; i < str.size(); i++) {
        HuffTree[str[i]].weight++;
    }

    //�������岿��
    for (int i = n; i < n * 2 - 1; i++) {
        int lc = -2, rc = -2;
        //�ҵ���С�������ڵ�
        selectHuffmanNode(HuffTree, lc, rc, n);
        if (i == n && (lc == -2 || rc == -2)) {
            cerr << "�ļ��ַ����ڵ�һ����֧��ѹ��" << endl;
            exit(0);
        }

        if (HuffTree[lc].parent != -1 || HuffTree[rc].parent != -1)
            break;

        HuffTree[lc].parent = i;
        HuffTree[rc].parent = i;
        HuffTree[i].lchild = lc;
        HuffTree[i].rchild = rc;
        HuffTree[i].weight = HuffTree[lc].weight + HuffTree[rc].weight;
    }

    //��ȡ���ڵ�
    int root = -1;
    for (int i = 0; i < n * 2 - 1; i++) {
        if (HuffTree[i].parent == -1 && HuffTree[i].weight != 0) {
            root = i;
        }
    }

    //���ù���������
    setHuffmanCode(HuffTree, root, "");
    return root;
}