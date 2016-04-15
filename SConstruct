# coding: utf8

import os

sysname = os.uname()[0]
if sysname not in ['FreeBSD', 'Linux']:
	print("不支持当前操作系统:", sysname)
	pinnt("支持的操作系统为: [FreeBSD, Linux]")

env = Environment()
env['ENV']['TERM'] = os.environ['TERM']

cpppath = ['.', ]
libpath = ['.', ]
libs = ['c++','icui18n', 'icuuc', 'chardet', 'iconv', ]

if sysname == 'FreeBSD':
	cpppath.append('/usr/local/include')
	cpppath.append('/depends/include/chardet')

	libpath.append('/usr/local/lib')
	libpath.append('/depends/lib')

env.Program(
	source = ['iconv-demo.c', ],
	target = 'c',
	CPPPATH = cpppath,
	LIBPATH = libpath,
	LIBS = libs,
)
