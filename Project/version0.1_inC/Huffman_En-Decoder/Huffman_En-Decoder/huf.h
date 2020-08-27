#ifndef _HUF_H
#define _HUF_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* 提供UNLONG_MAX常数, 用于检测整型数据数据类型的表达值范围 */


#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR -1 /* 文件读取失败(文件不存在) */
#define FAILED -2 /* malloc()函数申请空间失败 */
#define MAXCHARTYPE 256 /* 8bits位串最多可能产生256种字符 */
#define CHOSEN 1; /* 构建Huffman树过程中标记已被选过的节点 */


/* Huffman树 */
typedef struct {
    unsigned char ch; /* 以8位为一个单元存储字符 */
    unsigned long freq; /* 字符频度（次数） */
    char *huf_code; /* 字符对应Huffman码 */
    int parent, lchild, rchild; /* 双亲和左右孩子 */
} HufTreeNode, *HufTree;


/* 采用数组保存字符并统计字频，以下定义存储字符频度的节点 */
typedef struct {
    /* 定义字符频度表及其对应元素（节点） */
    unsigned char ch; /* 以8位为一个单元存储字符 */
    unsigned long freq; /* 字符频度（次数） */
} CharFreqNode, *CharFreqTable;


void OutputInfo(); /* 打印输出信息 */
int GetFrequency(char *infile_name, CharFreqTable *t, unsigned int *num, unsigned int *len); /* 获取字符频度 */
int SelectNode(HufTree huf_tree, unsigned int n, int *minimum, int *second_minimum); /* 返回两个最小结点 */
void SortTable(CharFreqTable *t); /* 按频度对字符表排序，使用冒泡排序 */
int CreateHufTree(HufTree huf_tree, unsigned int char_type_num, unsigned int node_num); /* 生成Huffman树 */
int CreateHufCode(HufTree huf_tree, unsigned int char_type_num); /* 生成Huffman码 */
int HuffmanEncoder(char *infile_name, char *outfile_name); /* 编码器 */
int HuffmanDecoder(char *infile_name, char *outfile_name); /* 解码器 */


#endif