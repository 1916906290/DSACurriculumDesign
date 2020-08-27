# !/usr/bin/env python
# ! -*- coding: utf-8 -*-
# @title: Huffman En-Decoder
# @author: _linxinhui_
# @version: 0.2


import os
import sys
import tkinter.filedialog


# 调整最大递归深度
# 当前系统的默认值为1000, 压缩大文件时可能引发RuntimeError: maximum recursion depth exceeded
sys.setrecursionlimit(1000000)


# Huffman树节点类
# 将单个节点定义为一个类以简化操作
class Node(object):
    # __init__是一个对象方法, 注意self参数
    def __init__(self, value = None, lchild = None, rchild = None, parent = None):
        self.value = value
        self.lchild = lchild
        self.rchild = rchild
        self.parent = parent

    def build_parent_node(lchild, rchild):
        node = Node(value = lchild.value + rchild.value, lchild = lchild, rchild = rchild)
        lchild.parent = node
        rchild.parent = node
        return node

    def encode_node(node):
        if node.parent == None:
            return b''
        if node.parent.lchild == node:
            return Node.encode_node(node.parent) + b'0'
        else:
            return Node.encode_node(node.parent) + b'1'


"""
自定义输入异常类
class InputError(Exception):
    def __init__(self, message):
        super().__init__(message)

当选择的操作不存在时, 抛出异常:
info = 'Unknow command, routine has not started doing anything, please run it again!'
raise InputError(info)

这里并没有采用抛出自定义异常的原因是解释器会自动抛出异常, 且异常信息与info大致相同
"""



def interactive_info():
    os.system("clear")
    print("-----Huffman En-Decoder-----")
    print("@author: _linxinhui_")
    print("@version: 0.2")
    print("\nOperations are as follows:\n1. Compress\n2. Decompress\n3. Quit")


def get_request():
    interactive_info()

    flag = int(input("Please choose an operation(enter the number): "))

    if flag == 1:
        root = tkinter.Tk()
        root.withdraw()
        infile_name = tkinter.filedialog.askopenfilename()
        huffman_encoder(infile_name)
    elif flag == 2:
        root = tkinter.Tk()
        root.withdraw()
        infile_name = tkinter.filedialog.askopenfilename()
        huffman_decoder(infile_name)
    elif flag == 3:
        quit()
    else:
        print("Input error, please try again!")


# 递归构建Huffman树
def build_huffman_tree(li):
    if len(li) == 1:
        return li
    # sorted(iterable, key = None, reverse = False)
    # 这里的key是用来比较的元素, 指定可迭代对象中的一个元素来进行排序
    sorted_li = sorted(li, key = lambda x: x.value, reverse = False)
    parent = Node.build_parent_node(sorted_li[0], sorted_li[1])
    sorted_li.pop(0)
    sorted_li.pop(0)
    sorted_li.append(parent)
    return build_huffman_tree(sorted_li)


# 生成Huffman码
def create_huffman_code(flag):
    for elem in leaf_nodes_dict.keys():
        huf_code_dict[elem] = Node.encode_node(leaf_nodes_dict[elem])
        # 输出编码表供调试
        if flag == True:
            print(elem, end = ': ')
            print(huf_code_dict[elem])


# 编码器
def huffman_encoder(infile_name):
    print('Starting compress, please wait......')

    bytes_width = 1  # 每次读取的字符宽度(字节数)
    huf_nodes = []  # 节点列表, 用于构建Huffman树

    # 读文件
    # open(name[, mode[, buffering]])
    infile = open(infile_name, 'rb')
    # fileObject.seek(offset[, whence])
    infile.seek(0, 2)
    # tell() 方法返回文件的当前位置, 即文件指针当前位置(相对于文件头)
    file_len = infile.tell() / bytes_width
    print("\nfile length: %d" % file_len)
    infile.seek(0)

    # 建立频度(权值)表
    i = 0
    buffer = [b''] * int(file_len)
    while i < file_len:
        # fileObject.read(size)用于从文件读取指定的字节数
        buffer[i] = infile.read(bytes_width)
        # dict.get(key, default = None)
        if char_freq_dict.get(buffer[i], -1) == -1:
            char_freq_dict[buffer[i]] = 0
        char_freq_dict[buffer[i]] += 1
        i = i + 1
    print('\nRead infile OK!')

    # 关闭文件
    infile.close()

    # 输出频度表(权值字典)供调试
    print('\nCharFreqTable ready:')
    print(char_freq_dict)

    # 将频度表拷贝的叶子节点, 构建Huffman树
    for elem in char_freq_dict.keys():
        leaf_nodes_dict[elem] = Node(char_freq_dict[elem])
        huf_nodes.append(leaf_nodes_dict[elem])
    huf_tree = build_huffman_tree(huf_nodes)

    # 生成Huffman码
    print('\nHuffmanCode ready:')
    create_huffman_code(True)
    print('\nEncode file OK!')

    # 动态选择编码表宽度(优化文件头)
    # items()函数以列表返回可遍历的(键, 值)元组数组
    head = sorted(char_freq_dict.items(), key = lambda x: x[1], reverse = True) # 提取dict中的键值对(key-value), 按value从大到小排列
    # head[0][1]即是文件中出现次数最多的字符出现的次数
    print("\nhighest frequency(aka head): %d" % head[0][1])
    bit_width = 1
    if head[0][1] > 16777215: # 16777215 = 2e24 - 1
        bit_width = 4
    elif head[0][1] > 65535: # 65535 = 2e16 - 1
        bit_width = 3
    elif head[0][1] > 255: # 255 == 2e8 - 1
        bit_width = 2
    print("thus, bit_width: %d" % bit_width)

    # 写入文件头(文件信息)
    # str.split(str = "", num = string.count(str)), split()方法通过指定分隔符对字符串进行切片, 如果参数num有指定值, 分隔num+1个子字符串
    outfile_name = infile_name.split('.')
    outfile_name = outfile_name[0] + '.huf'
    outfile = open(outfile_name, 'wb')
    outfile_name = infile_name.split('/')
    # fileObject.write( [ str ]), write()方法用于向文件中写入指定字符串, 若文件打开模式带有'b', 那么写入内容时, str要用encode方法转为bytes
    # 写文件名
    outfile.write((outfile_name[len(outfile_name) - 1] + '\n').encode(encoding = 'utf-8'))
    # 写节点数量
    outfile.write(int.to_bytes(len(huf_code_dict), 2, byteorder = 'big'))
    # 写编码表宽度
    outfile.write(int.to_bytes(bit_width, 1, byteorder = 'big'))
    # 写入文件头(编码表)
    for elem in huf_code_dict.keys():
        outfile.write(elem)
        outfile.write(int.to_bytes(char_freq_dict[elem], bit_width, byteorder = 'big'))
    print('\nWrite head OK!')

    # 编码
    i = 0
    raw = 0b1  # 二进制'1'
    last = 0
    while i < file_len:
        for elem in huf_code_dict[buffer[i]]:
            raw = raw << 1
            if elem == 49: # 49 == b'1'
                raw = raw | 1
            if raw.bit_length() == 9:
                raw = raw & (~(1 << 8))
                outfile.write(int.to_bytes(raw, 1, byteorder = 'big'))
                outfile.flush()
                raw = 0b1
                temp = int(i / len(buffer) * 100)
                if temp > last:
                    print('compressing: ', temp, '%')
                    last = temp
        i = i + 1

    # 处理文件最后的不足一个字节的数据
    if raw.bit_length() > 1:
        raw = raw << (8 - (raw.bit_length() - 1))
        raw = raw & (~(1 << raw.bit_length() - 1))
        outfile.write(int.to_bytes(raw, 1, byteorder = 'big'))

    # 关闭文件
    outfile.close()

    print('Successfully compressed!')


# 解码器
def huffman_decoder(infile_name):
    print("Starting decompress, please wait......")

    # 打开源文件并获取文件长度
    infile = open(infile_name, 'rb')
    infile.seek(0, 2)
    eof = infile.tell()
    infile.seek(0)

    # 读取文件名并构建outfile_name
    name = infile_name.split('/')
    out = infile.readline().decode(encoding="utf-8")
    out = out.replace('\n', '')
    out = out.split('.')
    out = out[0] + '_out.' + out[1]
    outfile_name = infile_name.replace(name[len(name) - 1], out)
    # 打开文件
    outfile = open(outfile_name, 'wb')
    # 读取节点数量
    count = int.from_bytes(infile.read(2), byteorder='big')
    # 读取编码表宽度
    bit_width = int.from_bytes(infile.read(1), byteorder='big')
    # 解析文件头, 获取频度表
    i = 0
    decode_dict = {}
    while i < count:
        key = infile.read(1)
        value = int.from_bytes(infile.read(bit_width), byteorder='big')
        decode_dict[key] = value
        i = i + 1

    print(decode_dict)

    if count == 1:
        for key in decode_dict.keys():
            i = key
        for value in decode_dict.values():
            j = value
        i = i * j
        outfile.write(i)
        return None

    # 重建Huffman树
    for elem in decode_dict.keys():
        leaf_nodes_dict[elem] = Node(decode_dict[elem])
        huf_nodes.append(leaf_nodes_dict[elem])
    huf_tree = build_huffman_tree(huf_nodes) 

    # 生成Huffman码
    create_huffman_code(True)

    # 交换huf_code_dict中的键和值(求逆)
    for elem in huf_code_dict.keys():
        inverse_dict[huf_code_dict[elem]] = elem

    print(inverse_dict)

    # 解压缩
    i = infile.tell()
    raw = 0
    data = b''
    # last = 0
    while i < eof:
        raw = int.from_bytes(infile.read(1), byteorder='big')
        print("raw: ",raw)
        i = i + 1
        j = 8
        while j > 0:
            if (raw >> (j - 1)) & 1 == 1:
                data = data + b'1'
                raw = raw & (~(1 << (j - 1)))
            else:
                data = data + b'0'
                raw = raw & (~(1 << (j - 1)))
            if inverse_dict.get(data, 0) != 0:
                outfile.write(inverse_dict[data])
                outfile.flush()
                print('decode ', data, ' :', inverse_dict[data])
                data = b''
            j = j - 1
        """
        temp = int(i / eof * 100)
        if temp > last:
            print("decompressing: ", temp, '%')  # 输出解压进度
            last = temp
        """
        raw = 0

    # 关闭文件
    infile.close()
    outfile.close()

    print("Successfully decompressed!")
    

if __name__ == "__main__":

    # 初始化变量
    leaf_nodes_dict = {}  # 原数据与编码节点的映射(叶子节点)
    char_freq_dict = {}  # 字符频度表(字典)
    huf_code_dict = {}  # Huffman编码字典
    inverse_dict = {}  # 反转huf_code_dict中的键值对
    huf_nodes = []  # Huffman树

    get_request()
    while input('Do you want to continue?(y/n): ').lower() == 'y':
        get_request()
