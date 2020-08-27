/*
 * NOTE:
 * This is not the Huffman En-Decoder but a simple huffman-coding demo.
 * General course assignment created several months ago(actually HW14.Q2_HuffmanTree)
 * For display only, no practical function
 * @author: _linxinhui_
 * @version: 0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 5 /* Huffman树的叶结点数目，也及给定的权值数目 */
#define M 9 /* 创建的Huffman树的结点数目 */

/* 给定的五个权值为{2, 4, 1, 3, 5} */
/* 画出初始化存储空间状态和构建好后存储空间的状态以及构建好的Huffman树 */
typedef char HTElemType;
typedef struct {
    /* Huffman树结点类型 */
    HTElemType data; 
    int weight;
    int parent, lchild, rchild;
}HTNode;
/* 静态三叉链表存储Huffman树，注意0号单元空出不用 */
HTNode HuffmanTree[M + 1];

/* 定义Huffman编码的数据类型 */
typedef char** HuffmanCode;
HuffmanCode hcd; /* hcd是huffman code的简写 */

void OutputInfo(); /* 打印部分信息 */
void CreatHuffmanTree(HTNode HuffmanTree[], int weight[]); /* 创建Huffman树 */
void InitHuffmanTreeSuccessfully(HTNode HuffmanTree[]); /* 初始化完成后存储空间的状态 */
void CreatHuffmanTreeSuccessfully(HTNode HuffmanTree[]); /* 创建完成后存储空间的状态 */
void FindTwoSmallestElem(HTNode HuffmanTree[], int *index_1, int *index_2); /* 找到两个权值最小的结点 */
void HuffmanCodeGenerator(HTNode HuffmanTree[], HuffmanCode *hcd); /* 求Huffman编码 */

int main () {
    /* 用户输入权数和权值，初始化N、M值并将权值读入weight数组 */
    int i, j;
    int weight[N] = {2, 4, 1, 3, 5};
    OutputInfo();
    CreatHuffmanTree(HuffmanTree, weight);
    HuffmanCodeGenerator(HuffmanTree, &hcd);
    printf("-------Huffman Code-------\n");
    for (i = 1; i <= N; ++i) {
        printf("%c:", HuffmanTree[i].data);
        for (j = 0; hcd[i][j] != '\0'; ++j) {
            printf("%c", hcd[i][j]);
        }
        printf("\n");
    }
    return 0;
}

void OutputInfo() {
    system("clear");
    printf("-------Huffman Code Demo-------\n");
    printf("@author: _linxinhui_\n");
    printf("@version: 0.0\n\n");
    printf("InformationSource:\n{'A':2, 'B':4, 'C':1, 'D':3, 'E':5}\n\n");
    return ;
}

void CreatHuffmanTree (HTNode HuffmanTree[], int weight[]) {
    int i, index_1, index_2;
    /* 初始化Huffman树 */
    for (i = 1; i <= M; ++i) {
        if (i >= 1 && i <= N) {
            HuffmanTree[i].weight = weight[i - 1];
        }
        HuffmanTree[i].data = 'A' + i - 1;
        HuffmanTree[i].parent = 0;
        HuffmanTree[i].lchild = 0;
        HuffmanTree[i].rchild = 0;
    }
    InitHuffmanTreeSuccessfully (HuffmanTree);
    /* 构造非叶子结点 */
    for (i = N + 1; i <= M; ++i) {
        FindTwoSmallestElem (HuffmanTree, &index_1, &index_2);
        HuffmanTree[i].weight = HuffmanTree[index_1].weight + HuffmanTree[index_2].weight;
        HuffmanTree[i].lchild = index_1;
        HuffmanTree[i].rchild = index_2;
        HuffmanTree[index_1].parent = i;
        HuffmanTree[index_2].parent = i; 
    }
    CreatHuffmanTreeSuccessfully(HuffmanTree);
    return ;
}

void FindTwoSmallestElem (HTNode HuffmanTree[], int *index_1, int *index_2) {
    int i, min_weight;
    for (min_weight = 65535, i = 1; i <= M; ++i) {
        /* 找到权值最小的（叶子）结点 */
        if (HuffmanTree[i].parent == 0 && HuffmanTree[i].weight != 0 && HuffmanTree[i].weight <= min_weight) {
            min_weight = HuffmanTree[i].weight;
            *index_1 = i;
        }
    }
    for (min_weight = 65535, i = 1; i <= M; ++i) {
        /* 找到权值次小的（叶子）结点 */
        if (HuffmanTree[i].parent == 0 && HuffmanTree[i].weight != 0 && i != *index_1 && HuffmanTree[i].weight <= min_weight) {
            min_weight = HuffmanTree[i].weight;
            *index_2 = i;
        }
    }
    return ;
}

void HuffmanCodeGenerator(HTNode HuffmanTree[], HuffmanCode *hcd) {
    /* 由Huffman树求N个结点的Huffman编码，注意由于路径顺序为由叶子结点到根结点，编码顺序为从下到上（从右到左） */
    int i, start, parent, cur;
    char *code;
    *hcd = (HuffmanCode)malloc ((N + 1) * sizeof(char *)); /* 指针（指向字符串）数组空间 */
    code = (char *)malloc (N * sizeof (char)); /* 保存（注意是暂存）编码（字符串） */ 
    for (i = 1; i <= N; ++i) {
        start = N - 1;
        code[start] = '\0';
        cur = i;
        parent = HuffmanTree[cur].parent;
        while (parent != 0) {
            if (HuffmanTree[parent].lchild == cur) {
                /* 左孩子的情形 */
                --start;
                code[start] = '0';
            }
            else {
                /* 右孩子的情形 */
                --start;
                code[start] = '1';
            }
            cur = parent;
            parent = HuffmanTree[cur].parent;
        }
        (*hcd)[i] = (char *)malloc ((N - start) * sizeof (char));
        strcpy((*hcd)[i], &code[start]);
    }
    return ;
}

void InitHuffmanTreeSuccessfully(HTNode HuffmanTree[]) {
    int i;
    printf("-------Initialize Huffman Tree Successfully-------\n");
    printf("data--weigth--parent--lchild--rchild\n");
    for (i = 1; i <= M; ++i) {
        printf("%4c%8d%8d%8d%8d\n", HuffmanTree[i].data, HuffmanTree[i].weight, HuffmanTree[i].parent, HuffmanTree[i].lchild, HuffmanTree[i].rchild);
    }
    printf("\n");
    return ;
}

void CreatHuffmanTreeSuccessfully(HTNode HuffmanTree[]) {
    int i;
    printf("-------Creat Huffman Tree Successfully-------\n");
    printf("data--weigth--parent--lchild--rchild\n");
    for (i = 1; i <= M; ++i) {
        printf("%4c%8d%8d%8d%8d\n", HuffmanTree[i].data, HuffmanTree[i].weight, HuffmanTree[i].parent, HuffmanTree[i].lchild, HuffmanTree[i].rchild);
    }
    printf("\n");
    return ;
}