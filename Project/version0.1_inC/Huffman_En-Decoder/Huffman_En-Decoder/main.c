#include "huf.h"


int main() {
    char infile_name[256], outfile_name[256]; /* 输入输出文件名 */
    int option, flag; /* option选择操作, flag判定En/Decode状态 */
    char ch[2]; /* 用gets(ch)达到getchar()等待键盘输入的效果 */
    
    while (TRUE) {
        flag = 0;
    /* 选择操作 */
    LOOP:
        OutputInfo();
        fflush(stdin); /* 清空标准输入缓冲区, 下同从略 */
        scanf("%d", &option);

        if (option == 3) {
            /* 直接退出 */
            printf("The program has run to completion.\n");
            printf("Thanks for using this program!\n");
            break;
        }
        else if (option == 1 || option == 2) {
            /* 读取输入输出文件的（绝对）路径 */
            printf("Please enter the absolute path of the input file:\n");
            fflush(stdin);
            gets(infile_name);
            printf("Please enter the absolute path of the output file:\n");
            fflush(stdin);
            gets(outfile_name);
        }
        else {
            /* 输入错误 */
            printf("The selected function does not exist, please try again!\n");
            printf("Press any key to continue...\n");
            fflush(stdin);
            gets(ch);
            goto LOOP;
        }

        /* 选择操作 */
        switch (option) {
            case 1:
                printf("Checking input validity, please wait......\n");
                flag = HuffmanEncoder(infile_name, outfile_name);
                if (flag == ERROR) {
                    printf("Error(file not found), please try again!\n");
                    printf("Press any key to continue...");
                    fflush(stdin);
                    gets(ch);
                    goto LOOP;
                }
                printf("Compressing, please wait......\n");
				printf("Operation completed!\n");
				printf("Press any key to continue...");
                fflush(stdin);
                gets(ch);
                break;
            case 2:
                printf("Checking input validity, please wait......\n");
                flag = HuffmanDecoder(infile_name, outfile_name);
                if (flag == ERROR) {
                    printf("Error(file not found), please try again!\n");
                    printf("Press any key to continue...");
                    fflush(stdin);
                    gets(ch);
                    goto LOOP;
                }
                printf("Extracting, please wait......\n");
				printf("Operation completed!\n");
                printf("Press any key to continue...");
                fflush(stdin);
                gets(ch);
                break;
        }
    }

    system("pause");
    return 0;
}


void OutputInfo() {
    /* 清屏, macOS/Linux下使用system("clear"); */
    system("cls");

    /* 简单交互 */
    printf("-------Huffman En-Decoder-------\n");
    printf("@author: _linxinhui_\n");
    /* Other Info:
    printf("@class: CS1901\n");
    printf("@No: 120191080104\n"); */
    printf("@version: 0.1\n\n");
    printf("Operations are as follows:\n1. To Compress\n2. To Extract\n3. To Quit\n");
    printf("Please choose an operation(enter the number): ");

    return ;
}


int GetFrequency(char *infile_name, CharFreqTable *t, unsigned int *num, unsigned int *len) {
    FILE *infile;
    unsigned char temp_char; /* 暂存字符（8bits位串） */
    int i;

    /* 创建顺序表, 用8位存储字符, 最多能存256种字符, 数组/顺序表长度置为256 */
    *t = (CharFreqTable)malloc(MAXCHARTYPE * sizeof(CharFreqNode));
    if (*t == NULL) {
        printf("Space applying failed, please try again.");
        return FAILED;
    }

    /* 初始化暂存字符节点 */
    for (i = 0; i < 256; ++ i) {
        /* 数组的256个下标与（可能的）256个字符对应 */
        (*t)[i].ch = (unsigned char)i;
        (*t)[i].freq = 0;
    }

    /* 打开文件 */
    infile = fopen(infile_name, "rb");
    if (infile == NULL) {
        return ERROR;
    }

    /* 读文件, 获取字符频度 */
    /* fread(buffer_pointer, size, count, file_pointer) */
    fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
    while (!feof(infile)) {
        ++ (*t)[(int)temp_char].freq;
        ++ (*len);
        fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
    }

    /* 关闭文件 */
    fclose(infile);

    /* 对table按频度排序 */
    SortTable(t);

    /* 统计实际字符种类, 并剔除频度为0的字符 */
    for (i = 0; i < MAXCHARTYPE; ++ i) {
        if ((*t)[i].freq == 0) {
            break;
        }
    }
    (*num) = i;

    return OK;
}


void SortTable(CharFreqTable *t) {
    int i, j;
    CharFreqNode temp;

    /* 按节点频度冒泡排序 */
    for (i = 0; i < MAXCHARTYPE - 1; ++ i) {
        for (j = i + 1; j < MAXCHARTYPE; ++ j) {
            if ((*t)[i].freq < (*t)[j].freq) {
                temp = (*t)[i];
                (*t)[i] = (*t)[j];
                (*t)[j] = temp;
            }
        }
    }

    return ;
}


int SelectNode(HufTree huf_tree, unsigned int n, int *minimum, int *second_minimum) {
    unsigned int i;
    unsigned long min;

    /* 找最小节点 */
    min = ULONG_MAX; /* ULONG_MAX是unsigned long int的十进制最大值 */
    for (i = 0; i < n; ++ i) {
        if (huf_tree[i].parent == 0 && huf_tree[i].freq < min) {
            min = huf_tree[i].freq;
            *minimum = i;
        }
    }
    huf_tree[*minimum].parent = CHOSEN;

    /* 找次小节点 */
    min = ULONG_MAX;
    for (i = 0; i < n; ++ i) {
        if (huf_tree[i].parent == 0 && huf_tree[i].freq < min) {
            min = huf_tree[i].freq;
            *second_minimum = i;
        }
    }

    return OK;
}


int CreateHufTree(HufTree huf_tree, unsigned int char_type_num, unsigned int node_num) {
    unsigned int i;
    int minimum, second_minimum;

    for (i = char_type_num; i < node_num; ++ i) {
        SelectNode(huf_tree, i, &minimum, &second_minimum); /* 选取两个频次最小的节点 */
        huf_tree[i].lchild = minimum;
        huf_tree[i].rchild = second_minimum;
        huf_tree[i].freq = huf_tree[minimum].freq + huf_tree[second_minimum].freq;
        huf_tree[minimum].parent = i;
        huf_tree[second_minimum].parent = i;
    }

    return OK;
}


int CreateHufCode(HufTree huf_tree, unsigned int char_type_num) {
    unsigned int i;
    int cur, parent;
    int index;
    char *temp_code = NULL;

    temp_code = (char *)malloc(MAXCHARTYPE * sizeof(char));
    temp_code[MAXCHARTYPE - 1] = '\0';

    for (i = 0; i < char_type_num; ++ i) {
        index = MAXCHARTYPE - 1;
        cur = i;
        parent = huf_tree[cur].parent;

        /* 从叶子节点到根节点, 反向生成Huffman码 */
        while (parent != 0) {
            if (huf_tree[parent].lchild == cur) {
                /* 左分支编'0' */
                index -= 1;
                temp_code[index] = '0';
            }
            else {
                /* 左分支编'1' */
                index -= 1;
                temp_code[index] = '1';
            }

            cur = parent;
            parent = huf_tree[cur].parent;
        }

        /* 将生成的编码保存到树节点 */
        huf_tree[i].huf_code = (char *)malloc((MAXCHARTYPE - index) * sizeof(char));
        strcpy(huf_tree[i].huf_code, &temp_code[index]); /* 正向保存Huffman码 */
    }

    /* 释放临时编码占用的空间 */
    free(temp_code);

    return OK;
}


int HuffmanEncoder(char *infile_name, char *outfile_name) {
    FILE *infile, *outfile;
    CharFreqTable table; /* 字符频度表, 统计节点频度并拷贝到树节点之后即被释放 */
    unsigned char temp_char; /* 暂存字符(8bits位串) */
    unsigned int char_type_num; /* 字符(8bits位串)种类数 */
    unsigned int file_len = 0; /* 文件长度 */
    unsigned int node_num; /* Huffman树中将会有的节点数, 由叶子节点数(字符数)确定 */
    HufTree huf_tree;
    char buffer[MAXCHARTYPE] = "\0"; /* 编码缓冲区 */
    unsigned int code_len;
    unsigned int i;

    /* 读源文件, 统计词频, 存在table表中 */
    if (GetFrequency(infile_name, &table, &char_type_num, &file_len) == ERROR) {
        return ERROR;
    }

    /* 若只有一种字符 */
    if (char_type_num == 1) {
        /* 打开文件 */
        outfile = fopen(outfile_name, "wb");
        if (outfile == NULL) {
            return ERROR;
        }

        /* 写入字符数、字符、字符频度(表) */
        fwrite((char *)(&char_type_num), sizeof(unsigned int), 1, outfile);
        fwrite((char *)(&table[0].ch), sizeof(unsigned char), 1, outfile);
        fwrite((char *)(&table[0].freq), sizeof(unsigned long), 1, outfile);

        free(table);

        fclose(outfile);
    }
    /* 若不止一种字符 */
    else {
        node_num = 2 * char_type_num - 1;

        huf_tree = (HufTree)malloc(node_num * sizeof(HufTreeNode));

        /* 初始化Huffman树的前char_type_num个节点 */
        for (i = 0; i < char_type_num; ++ i) {
            /* 将table的内容拷贝到树节点 */
            huf_tree[i].ch = table[i].ch;
            huf_tree[i].freq = table[i].freq;
            huf_tree[i].parent = 0;
        }

        /* 释放table */
        free(table);

        /* 初始化Huffman树的剩余节点 */
        for (i = char_type_num; i < node_num; ++ i) {
            huf_tree[i].parent = 0;
        }

        /* 构建Huffman树 */
        CreateHufTree(huf_tree, char_type_num, node_num);

        /* 生成Huffman码 */
        CreateHufCode(huf_tree, char_type_num);

        /* 开始写文件 */
        outfile = fopen(outfile_name, "wb");
        if (outfile == NULL) {
            return ERROR;
        }

        /* 向文件写入字符及其频度 */
        fwrite((char *)(&char_type_num), sizeof(unsigned int), 1, outfile);
        for (i = 0; i < char_type_num; ++ i) {
            fwrite((char *)(&huf_tree[i].ch), sizeof(unsigned char), 1, outfile);
            fwrite((char *)&huf_tree[i].freq, sizeof(unsigned long), 1, outfile);
        }

        /* 向文件写入文件长度 */
        fwrite((char *)(&file_len), sizeof(unsigned long), 1, outfile);

        /* 字符编码 */
        infile = fopen(infile_name, "rb");
        if (infile == NULL) {
            return ERROR;
        }
        fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
        while (!feof(infile)) {
            /* 匹配从infile读取的字符和存储在Huffman树中的字符, 并将对应Huffman码复制到buffer */
            for (i = 0; i < char_type_num; ++ i) {
                if (temp_char == huf_tree[i].ch) {
                    strcat(buffer, huf_tree[i].huf_code);
                }
            }

            /* 以8个长度(这里的长度既可以指bit, 也可指byte)为单元编码 */
            while (strlen(buffer) >= 8) {
                temp_char = '\0'; /* '\0'等价于(0x00)_16等价于(00000000)_2 */
                for (i = 0; i < 8; ++ i) {
                    temp_char <<= 1;
                    if (buffer[i] == '1') {
                        temp_char |= 1;
                    }
                }
                fwrite((char *)(&temp_char), sizeof(unsigned char), 1, outfile);
                strcpy(buffer, buffer + 8);
            }
            fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
        }
        
        /* 处理末尾buffer中(很有可能存在的)不足8个长度的编码 */
        code_len = strlen(buffer);
        if (code_len > 0) {
            temp_char = '\0';
            for (i = 0; i < code_len; ++ i) {
                temp_char = temp_char << 1;
                if (buffer[i] == '1') {
                    temp_char = temp_char | 1;
                }
            }
            temp_char <<= (8 - code_len);
            fwrite((char *)(&temp_char), sizeof(unsigned char), 1, outfile);
        }

        /* 关闭文件 */
        fclose(infile);
        fclose(outfile);

        /* 释放Huffman树占用的空间 */
        for (i = 0; i < char_type_num; ++ i) {
            free(huf_tree[i].huf_code);
        }
        free(huf_tree);
    }

    return OK;
}


int HuffmanDecoder(char *infile_name, char *outfile_name) {
    FILE *infile, *outfile;
    HufTree huf_tree;
    unsigned int root; /* 根节点地址 */
    unsigned int char_type_num;
    unsigned int node_num;
    unsigned char temp_char; /* 暂存8bits编码 */
    unsigned long file_len = 0;
    unsigned long writen_len = 0;
    unsigned int i;

    /* 打开文件 */
    infile = fopen(infile_name, "rb");
    if (infile == NULL) {
        return ERROR;
    }

    /* 读取文件头信息(字符数) */
    fread((char *)(&char_type_num), sizeof(unsigned int), 1, infile);

    /* 解码, 当只有一种字符时 */
    if (char_type_num == 1) {
        /* 读文件头信息(字符, 文件长度) */
        fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
        fread((char *)(&file_len), sizeof(unsigned int), 1, infile);

        /* 解码 */
        outfile = fopen(outfile_name, "wb");
        if (outfile == NULL) {
            return ERROR;
        }
        while (file_len > 0) {
            fwrite((char *)(&temp_char), sizeof(unsigned char), 1, outfile);
            -- file_len;
        }

        /* 关闭文件 */
        fclose(infile);
        fclose(outfile);
    }
    /* 解码, 当不止一种字符时 */
    else {
        node_num = 2 * char_type_num - 1;
        
        huf_tree = (HufTree)malloc(node_num * sizeof(HufTreeNode));

        /* 初始化Huffman树 */
        for (i = 0; i < char_type_num; ++ i) {
            fread((char *)(&huf_tree[i].ch), sizeof(unsigned char), 1, infile);
            fread((char *)(&huf_tree[i].freq), sizeof(unsigned long), 1, infile);
            huf_tree[i].parent = 0;
        }
        for (i = char_type_num; i < node_num; ++ i) {
            huf_tree[i].parent = 0;
        }

        /* 重建Huffman树 */
        CreateHufTree(huf_tree, char_type_num, node_num);

        /* 解码 */
        fread((char *)(&file_len), sizeof(unsigned long), 1, infile);
        outfile = fopen(outfile_name, "wb");
        if (outfile == NULL) {
            return ERROR;
        }
        root = node_num - 1;
        while (TRUE) {
            /* 以 8bits 为一个处理单元 */
            fread((char *)(&temp_char), sizeof(unsigned char), 1, infile);
            for (i = 0; i < 8; ++ i) {
                if ((temp_char & 128) == 0) {
                    /* 128等价于0b10000000 */
                    root = huf_tree[root].lchild;
                }
                else {
                    root = huf_tree[root].rchild;
                }
                if (root < char_type_num) {
                    fwrite((char *)(&huf_tree[root].ch), sizeof(unsigned char), 1, outfile);
                    ++ writen_len;
                    if (writen_len == file_len) {
                        break;
                    }
                    root = node_num - 1;
                }
                temp_char <<= 1;
            }
            if (writen_len == file_len) {
                break;
            }
        }

        /* 关闭文件 */
        fclose(infile);
        fclose(outfile);

        /* 释放Huffman树占用的空间 */
        free(huf_tree);
    }

    return OK;
}
