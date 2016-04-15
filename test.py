#!/usr/bin/env python
# coding: utf8

import array

with open('input.hexarray.txt', 'r') as f:
    s = f.read()
print('number:', s)

l = list(map(lambda x: int(x, 16), s.split()))

b = array.array('B', l).tobytes()
print('bytes:', b)
print("gb2312: ", b.decode("gb2312"))
print("gbk:    ", b.decode("gbk"))
print("gb18030:", b.decode("gb18030"))
print("utf8:   ", b.decode("utf8"))

with open('gb2312.txt', 'wb') as f:
	f.write(b)

output = array.array('B', l).tostring().decode('gb2312')
print('output, gb2312-decode:', output)

s = '正在安装'
print(s, 'utf8   ', s.encode('utf8'))
print(s, 'gb2312 ', s.encode('gb2312'))
print(s, 'gbk    ', s.encode('gbk'))
print(s, 'gb18030', s.encode('gb18030'))
