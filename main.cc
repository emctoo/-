#include <stdint.h>
#include <stdio.h>

#include <iostream>

#include <QString>

#include <glog/logging.h>
#include <cppformat/format.h>

#include <chardet/chardet.h>
#include <iconv.h>

// 接收到的字节, gb18030编码的, 要转换成utf8的
uint8_t bytes[] = {
	0x52, 0x45, 0x54, 0x52, 0x20, 0x2f, 0x70, 0x75, 0x62, 0x2f, 0x6d, 0x79, 0x74,
	0x6f, 0x6f, 0x6c, 0x73, 0x2f, 0x66, 0x69, 0x6c, 0x6d, 0x2f, 0xc1, 0xfa, 0xce,
	0xc6, 0xc9, 0xed, 0xb5, 0xc4, 0xc5, 0xae, 0xba, 0xa2, 0x2e, 0x54, 0x68, 0x65,
	0x2e, 0x47, 0x69, 0x72, 0x6c, 0x2e, 0x57, 0x69, 0x74, 0x68, 0x2e, 0x54, 0x68,
	0x65, 0x2e, 0x44, 0x72, 0x61, 0x67, 0x6f, 0x6e, 0x2e, 0x54, 0x61, 0x74, 0x74,
	0x6f, 0x6f, 0x2e, 0x32, 0x30, 0x30, 0x39, 0x2e, 0x42, 0x44, 0x2d, 0x52, 0x4d,
	0x56, 0x42, 0x2d, 0xc8, 0xcb, 0xc8, 0xcb, 0xd3, 0xb0, 0xca, 0xd3, 0xd4, 0xad,
	0xb4, 0xb4, 0xb7, 0xad, 0xd2, 0xeb, 0xd6, 0xd0, 0x25, 0x2e, 0x72, 0x6d, 0x76,
	0x62, 0x00}; // 添加了0结尾

void dump_cstring_bytes(const char *bytes, size_t sz) {
    fmt::print_colored(fmt::RED, "{}\n", ".... .... .... ....");

    printf("[%.*s]\n", (int)sz, bytes);
    for (size_t nth = 0; nth < sz; ++nth) {
        printf("%x, ", (uint8_t)bytes[nth]);
    }
    putchar('\n');

    fmt::print_colored(fmt::BLUE, "{}\n", "====");
}

int detect_bytes_charset(const char *inbuf, std::string &encoding) {
    DetectObj *obj = detect_obj_init();
    if (obj == NULL) {
        LOG(WARNING) << "内存分配错误";
        return -1;
    }

    switch (detect(inbuf, &obj)) {
    case CHARDET_OUT_OF_MEMORY :
        LOG(WARNING) << "处理错误: out of memory";
        detect_obj_free (&obj);
        return -1;

    case CHARDET_NULL_OBJECT :
        LOG(WARNING) << "处理错误: null object";
        return -1;
    }

    encoding = std::string(obj->encoding);
    detect_obj_free(&obj);

    return 0;
}

// 代码转换:从一种编码转为另一种编码
int convert_codec_to_utf8(const char *from_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
   char **pin = &inbuf, **pout = &outbuf;

   iconv_t cd = iconv_open("utf-8", from_charset); // 必须是utf-8, 不能是utf8
   if ((int64_t)cd == -1) {
       LOG(WARNING) << "创建转码结构失败: " << strerror(errno);
       return -1;
   }

   memset(outbuf, 0, outlen);

   size_t outbytesleft = outlen;
   size_t status = iconv(cd, pin, &inlen, pout, &outbytesleft);
   if ((int)status == -1) {
      LOG(WARNING) << fmt::format("转码失败, [{}]", strerror(errno));
      return -1;
   }

   iconv_close(cd);
   return outlen - outbytesleft;
}

int working(uint8_t *input_bytes, size_t size, char *output, int output_max_size) {
    uint8_t *the_rest_bytes = (uint8_t *)memchr(input_bytes, ' ', size); // 找空格的地方
    if (the_rest_bytes == NULL) the_rest_bytes = input_bytes + size; // 有可能找不到，就指向末端

    std::string command((char *)input_bytes, the_rest_bytes - input_bytes);
    LOG(INFO) << fmt::format("command: [{}]", command);

    if (the_rest_bytes == (input_bytes + size)) {
        LOG(INFO) << "command没有参数，不需要进一步处理";
        return -1;
    }

    the_rest_bytes++; // 跳过空格
    size_t the_rest_bytes_size = input_bytes + size - the_rest_bytes;

    std::string encoding;
    if (detect_bytes_charset((char *)the_rest_bytes, encoding) == -1) {
        LOG(WARNING) << "fail to detect encoding";
        return -1;
    }

    int output_bytes_size = convert_codec_to_utf8(
                encoding.c_str(), (char *)the_rest_bytes, the_rest_bytes_size, output, output_max_size);
    if (output_bytes_size == -1) return -1;

    return 0;
}

// uint8_t * -> char *
int main(int argc, char *argv[]) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;

    LOG(INFO) << "starting ...";

	char output[1024] = "";
    working(bytes, sizeof(bytes), output, 1024);

#if 0
	size_t length = sizeof(bytes);

	std::cout << "length: " << length << '\n';
	std::cout << bytes << '\n';

	for (size_t nth = 0; nth < length; ++nth) {
		printf("%x, ", bytes[nth]);
	}
	putchar('\n');

	printf("--------------------------------\n");

	char *s = reinterpret_cast<char *>(bytes);
	for (size_t nth = 0; nth < length; ++nth) {
		printf("%x, ", s[nth] > 0 ? s[nth] : 128 - s[nth]);
	}
	putchar('\n');

    QString S = QString::fromLocal8Bit(s, length).trimmed();
    std::cout << S.toStdString() << '\n';

    std::string command = S.toStdString();

    char *input_bytes = const_cast<char *>(command.c_str());
    dump_cstring_bytes(input_bytes, command.size());
#endif
    google::ShutdownGoogleLogging();
	return 0;
}
