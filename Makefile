all: c cpp

c: iconv-demo.c
	clang -g -o $@ $^ -licui18n -licuuc -lchardet -I/usr/include/chardet -luchardet

cpp: iconv-demo.cc
	clang++ -o $@ $^

clean:
	rm -rf c cpp *.o