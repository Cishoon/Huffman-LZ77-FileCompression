#define _CRT_SECURE_NO_WARNINGS
#include <queue>
#include <iostream>
#include "Huffmam.h"
using namespace std;


/***************************************************************************
  功    能：选出权重最小的两个哈夫曼编码
  输入参数：
            lc      :       权重最小的节点下标
            rc      :       权重第二小的节点下标
            n       :       哈夫曼树节点数量
  说    明：暴力枚举，
            尝试使用优先队列优化，结果发现效率还不如直接暴力，不知道是什么原因
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
  功    能：设置哈夫曼编码
  输入参数：
            HuffTree        :       哈夫曼树数组首地址
            p               :       下标
            bits            :       哈夫曼编码
  说    明：深搜
***************************************************************************/
void setHuffmanCode(HuffNode* HuffTree, int p, string bits)
{
    if (HuffTree[p].lchild == -1 && HuffTree[p].rchild == -1)  //如果这个点是叶子结点
    {
        HuffTree[p].bits = bits;
        return;
    }
    setHuffmanCode(HuffTree, HuffTree[p].lchild, bits + "0");
    setHuffmanCode(HuffTree, HuffTree[p].rchild, bits + "1");
}

/***************************************************************************
  功    能：建立哈夫曼树
  输入参数：
            HuffTree        :       哈夫曼树数字首地址
            str             :       储存文件所有内容的vector容器
            n               :       节点数量
  返 回 值：根节点下标
***************************************************************************/
int buildHuffTree(HuffNode HuffTree[], vector<unsigned char>& str, int n)
{
    //获取每个节点的权重
    for (vector<char>::size_type i = 0; i < str.size(); i++) {
        HuffTree[str[i]].weight++;
    }

    //建树主体部分
    for (int i = n; i < n * 2 - 1; i++) {
        int lc = -2, rc = -2;
        //找到最小的两个节点
        selectHuffmanNode(HuffTree, lc, rc, n);
        if (i == n && (lc == -2 || rc == -2)) {
            cerr << "文件字符过于单一，不支持压缩" << endl;
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

    //获取根节点
    int root = -1;
    for (int i = 0; i < n * 2 - 1; i++) {
        if (HuffTree[i].parent == -1 && HuffTree[i].weight != 0) {
            root = i;
        }
    }

    //设置哈夫曼编码
    setHuffmanCode(HuffTree, root, "");
    return root;
}