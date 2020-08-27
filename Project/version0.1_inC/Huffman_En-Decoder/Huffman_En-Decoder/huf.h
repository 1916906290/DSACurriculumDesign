#ifndef _HUF_H
#define _HUF_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* �ṩUNLONG_MAX����, ���ڼ�����������������͵ı��ֵ��Χ */


#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR -1 /* �ļ���ȡʧ��(�ļ�������) */
#define FAILED -2 /* malloc()��������ռ�ʧ�� */
#define MAXCHARTYPE 256 /* 8bitsλ�������ܲ���256���ַ� */
#define CHOSEN 1; /* ����Huffman�������б���ѱ�ѡ���Ľڵ� */


/* Huffman�� */
typedef struct {
    unsigned char ch; /* ��8λΪһ����Ԫ�洢�ַ� */
    unsigned long freq; /* �ַ�Ƶ�ȣ������� */
    char *huf_code; /* �ַ���ӦHuffman�� */
    int parent, lchild, rchild; /* ˫�׺����Һ��� */
} HufTreeNode, *HufTree;


/* �������鱣���ַ���ͳ����Ƶ�����¶���洢�ַ�Ƶ�ȵĽڵ� */
typedef struct {
    /* �����ַ�Ƶ�ȱ����ӦԪ�أ��ڵ㣩 */
    unsigned char ch; /* ��8λΪһ����Ԫ�洢�ַ� */
    unsigned long freq; /* �ַ�Ƶ�ȣ������� */
} CharFreqNode, *CharFreqTable;


void OutputInfo(); /* ��ӡ�����Ϣ */
int GetFrequency(char *infile_name, CharFreqTable *t, unsigned int *num, unsigned int *len); /* ��ȡ�ַ�Ƶ�� */
int SelectNode(HufTree huf_tree, unsigned int n, int *minimum, int *second_minimum); /* ����������С��� */
void SortTable(CharFreqTable *t); /* ��Ƶ�ȶ��ַ�������ʹ��ð������ */
int CreateHufTree(HufTree huf_tree, unsigned int char_type_num, unsigned int node_num); /* ����Huffman�� */
int CreateHufCode(HufTree huf_tree, unsigned int char_type_num); /* ����Huffman�� */
int HuffmanEncoder(char *infile_name, char *outfile_name); /* ������ */
int HuffmanDecoder(char *infile_name, char *outfile_name); /* ������ */


#endif