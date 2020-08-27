# !/usr/bin/env python
# ! -*- coding: utf-8 -*-
# @title: Huffman En-Decoder
# @author: _linxinhui_
# @version: 1.0

from tkinter import *
from tkinter import filedialog, messagebox
import sys

# 调整最大递归深度
sys.setrecursionlimit(100000000)


# Huffman树节点类
class Node(object):

    # __init__是一个对象方法, 注意self参数
    def __init__(self, value=None, lchild=None, rchild=None, parent=None):
        self.value = value
        self.lchild = lchild
        self.rchild = rchild
        self.parent = parent

    # 创建一个节点的双亲节点
    def build_parent_node(lchild, rchild):
        node = Node(value=lchild.value + rchild.value, lchild=lchild, rchild=rchild)
        lchild.parent = node
        rchild.parent = node
        return node

    # 对某个叶节点编码(递归)
    def encode_node(node):
        if node.parent == None:
            return b''
        if node.parent.lchild == node:
            return Node.encode_node(node.parent) + b'0'
        else:
            return Node.encode_node(node.parent) + b'1'


# 构建Huffman树(递归)
def build_huffman_tree(li):
    if len(li) == 1:
        return li

    sorted_li = sorted(li, key=lambda x: x.value, reverse=False)

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
        if flag:
            print(elem, end=': ')
            print(huf_code_dict[elem])


# 编码器, 由'Compress'button触发
def huffman_encoder():
    root = Tk()
    root.withdraw()
    # 弹出消息窗口, 确认是否进行压缩操作
    ask_title = 'Before moving on to the next step'
    ask_message = "Hello!\nNote that you are using the COMPRESSION function\nClick 'OK' button  to choose the input " \
                  "file and go on, click 'Cancel' to abort current operation and go back\n "

    flag = messagebox.askokcancel(title=ask_title, message=ask_message)
    if not flag:
        return
    # 弹出文件选择窗口
    infile_name = filedialog.askopenfilename(title='before compressing, choose the input file please')
    print(infile_name)

    # 弹出消息窗口, 提示开始压缩
    messagebox.showinfo(title=None, message="Starting compress, please wait......\n(Click 'OK' button to go on)")

    bytes_width = 1  # 每次读取的字符宽度(字节数)
    huf_nodes = []  # 节点列表, 用于构建Huffman树

    # 读文件, 统计文件长度
    infile = open(infile_name, 'rb')
    infile.seek(0, 2)
    file_len = infile.tell() / bytes_width
    print("\nfile length: %d" % file_len)
    infile.seek(0)

    # 建立频度(权值)表
    i = 0
    buffer = [b''] * int(file_len)
    while i < file_len:
        buffer[i] = infile.read(bytes_width)
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

    # 将频度表拷贝的叶子节点
    for elem in char_freq_dict.keys():
        leaf_nodes_dict[elem] = Node(char_freq_dict[elem])
        huf_nodes.append(leaf_nodes_dict[elem])

    # 构建Huffman树
    huf_tree = build_huffman_tree(huf_nodes)

    # 生成Huffman码, 打印码表供调试
    print('\nHuffmanCode ready:')
    create_huffman_code(True)
    print('\nEncode file OK!')

    # 预备写文件, 选择频度表宽度以优化文件头
    head = sorted(char_freq_dict.items(), key=lambda x: x[1], reverse=True)  # 提取dict中的键值对(key-value), 按value从大到小排列
    print("\nhighest frequency(aka head): %d" % head[0][1])  # head[0][1]即是文件中出现次数最多的字符出现的次数
    bit_width = 1
    if head[0][1] > 16777215:  # 16777215 = 2e24 - 1
        bit_width = 4
    elif head[0][1] > 65535:  # 65535 = 2e16 - 1
        bit_width = 3
    elif head[0][1] > 255:  # 255 == 2e8 - 1
        bit_width = 2
    print("thus, bit_width: %d" % bit_width)

    # 写入文件头, 包括文件信息和频度表两个主要部分
    outfile_name = infile_name.split('.')
    outfile_name = outfile_name[0] + '.huf'
    outfile = open(outfile_name, 'wb')
    outfile_name = infile_name.split('/')
    outfile.write((outfile_name[len(outfile_name) - 1] + '\n').encode(encoding='utf-8'))  # 写文件名
    outfile.write(int.to_bytes(len(huf_code_dict), 2, byteorder='big'))  # 写节点数量
    outfile.write(int.to_bytes(bit_width, 1, byteorder='big'))  # 写码表宽度
    # 写入频度表
    for elem in huf_code_dict.keys():
        outfile.write(elem)
        outfile.write(int.to_bytes(char_freq_dict[elem], bit_width, byteorder='big'))
    print('\nWrite head OK!')

    # 编码并写入文件
    i = 0
    raw = 0b1  # 二进制'1'
    # last = 0
    while i < file_len:
        for elem in huf_code_dict[buffer[i]]:
            raw <<= 1
            if elem == 49:  # 49 == b'1'
                raw |= 1
            if raw.bit_length() == 9:
                raw &= (~(1 << 8))
                outfile.write(int.to_bytes(raw, 1, byteorder='big'))
                outfile.flush()
                raw = 0b1
                """
                这部分的作用是打印编码进度, 在压缩小文件的情况下不能正常工作, 干脆省略, 下面解码器部分同
                temp = int(i / len(buffer) * 100)
                if temp > last:
                    print('compressing: ', temp, '%')
                    last = temp
                """
        i += 1
    # 处理文件最后的不足一个字节的数据
    if raw.bit_length() > 1:
        raw <<= (8 - (raw.bit_length() - 1))
        raw &= (~(1 << raw.bit_length() - 1))
        outfile.write(int.to_bytes(raw, 1, byteorder='big'))

    # 关闭文件
    outfile.close()

    # 弹出消息窗口, 提示压缩完成
    messagebox.showinfo(title='Done', message="Compress successfully, click 'OK' to go back to main window")


# 解码器, 由'Extract'button触发
def huffman_decoder():
    root = Tk()
    root.withdraw()
    # 弹出消息窗口, 确认是否进行解压缩操作
    ask_title = 'Before moving on to the next step'
    ask_message = "Hello!\nNote that you are using the DECOMPRESSION function\nClick 'OK' button  to choose the input " \
                  "file and go on, click 'Cancel' to abort current operation and go back\n"

    flag = messagebox.askokcancel(title=ask_title, message=ask_message)
    if not flag:
        return
    # 弹出文件选择窗口
    infile_name = filedialog.askopenfilename(title='before decompressing, choose the input file please')

    # 弹出消息窗口, 提示开始压缩
    messagebox.showinfo(title=None, message="Starting decompress, please wait......\n(Click 'OK' button to go on)")

    # 读文件, 统计文件长度
    infile = open(infile_name, 'rb')
    infile.seek(0, 2)
    eof = infile.tell()
    infile.seek(0)

    # 读文件, 读取文件头
    # 构建outfile_name
    name = infile_name.split('/')
    out = infile.readline().decode(encoding="utf-8")
    out = out.replace('\n', '')
    out = out.split('.')
    out = out[0] + '_out.' + out[1]
    outfile_name = infile_name.replace(name[len(name) - 1], out)
    count = int.from_bytes(infile.read(2), byteorder='big')  # 读取节点数量
    bit_width = int.from_bytes(infile.read(1), byteorder='big')  # 读取频度表宽度
    # 读取频度表并打印供测试
    i = 0
    decode_dict = {}
    while i < count:
        key = infile.read(1)
        value = int.from_bytes(infile.read(bit_width), byteorder='big')
        decode_dict[key] = value
        i += 1
    print(decode_dict)

    # 创建解压缩后的文件
    outfile = open(outfile_name, 'wb')

    # 处理原文件只有一个字符(创建Huffman树无意义, 无法生成Huffman码)的特殊情形
    if count == 1:
        for key in decode_dict.keys():
            i = key
        for value in decode_dict.values():
            j = value
        i = i * j
        outfile.write(i)
        return

    # 重建Huffman树
    for elem in decode_dict.keys():
        leaf_nodes_dict[elem] = Node(decode_dict[elem])
        huf_nodes.append(leaf_nodes_dict[elem])
    huf_tree = build_huffman_tree(huf_nodes)

    # 生成Huffman码, 输出供调试
    create_huffman_code(True)

    # 交换huf_code_dict中的键和值(求逆), 存储到反转码表中, 输出反转码表供调试
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
        print("raw: ", raw)
        i += 1
        j = 8  # 指定默认读码长度(实质上是缓冲区长度)
        while j > 0:
            # 读取到1
            if (raw >> (j - 1)) & 1 == 1:
                data += b'1'
                raw &= (~(1 << (j - 1)))
            # 读取到0
            else:
                data += b'0'
                raw = raw & (~(1 << (j - 1)))
            # 每读入一位就扫描一遍反转码表, 尝试解码, 解码成功写入文件, 否则继续纳入位
            if inverse_dict.get(data, 0) != 0:
                outfile.write(inverse_dict[data])
                outfile.flush()
                print('decode ', data, ' :', inverse_dict[data])
                data = b''
            j -= 1
        """
        temp = int(i / eof * 100)
        if temp > last:
            print("decompressing: ", temp, '%')
            last = temp
        """
        raw = 0

    # 关闭文件
    infile.close()
    outfile.close()

    # 弹出消息窗口, 提示解压缩完成
    messagebox.showinfo(title='Done', message="Compress successfully, click 'OK' to go back to main window")


# 'Quit'button触发这一函数, 退出程序
def program_quit():
    quit()


if __name__ == "__main__":
    # 初始化变量
    leaf_nodes_dict = {}  # 原数据-叶节点dict
    char_freq_dict = {}  # 字符-频度dict
    huf_code_dict = {}  # 叶节点-Huffman码dict
    inverse_dict = {}  # 反转huf_code_dict中的键值对
    huf_nodes = []  # Huffman树节点list

    # 创建主窗口
    window = Tk()
    window.title('Huffman En-Decoder')
    default_font = 'Monaco'
    width = 600
    height = 550

    # 将主窗口放置在屏幕正中, 并设定大小不可变
    screenwidth = window.winfo_screenwidth()
    screenheight = window.winfo_screenheight()
    alignstr = '%dx%d+%d+%d' % (width, height, (screenwidth - width) / 2, (screenheight - height) / 2)
    window.geometry(alignstr)
    window.resizable(width=False, height=False)

    # 创建画布, 创建图片, 放置图片
    canvas = Canvas(window, width=594, height=250, bg='black')
    image_file_huf = PhotoImage(file='pics/1.gif')
    image_file_intro = PhotoImage(file='pics/2.gif')
    image_huf = canvas.create_image(0, 125, anchor='w', image=image_file_huf)
    image_intro = canvas.create_image(186, 125, anchor='w', image=image_file_intro)
    canvas.place(x=0, y=0)

    # 主窗口提示信息
    basic_info = '---------Huffman En-Decoder---------\n\n@author: _linxinhui_\n@version: 1.0\n\nOperations are as ' \
                 'follows, click the buttons to choose:'
    func_info = '   1. Compress\n  2. Extract\n\b\b3. Quit '

    # 在主窗口上创建标签, 输出上述信息
    info = Label(window, text=basic_info, font=default_font, bg='white', fg='black')
    info.place(width=600, height=120, x=0, y=250)
    func = Label(window, text=func_info, font=default_font, bg='white', fg='black')
    func.place(width=580, height=100, x=0, y=370)

    # 创建按钮
    # 压缩
    btn_compress = Button(window, text='Compress', font=default_font, command=huffman_encoder)
    btn_compress.place(width=200, height=50, x=0, y=500)
    # 解压缩
    btn_extract = Button(window, text='Extract', font=default_font, command=huffman_decoder)
    btn_extract.place(width=200, height=50, x=200, y=500)
    # 退出
    btn_quit = Button(window, text='Quit', font=default_font, command=quit)
    btn_quit.place(width=200, height=50, x=400, y=500)

    # 进入消息循环
    window.mainloop()