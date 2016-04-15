#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include <iconv.h>
#include <unicode/ucsdet.h>

#include <chardet/chardet.h> // mozilla libchardet

#include <uchardet/uchardet.h> // libuchardet

void detect_bytes_charset_by_libuchardet(const char *data) {
   size_t len = strlen(data);

   uchardet_t ud = uchardet_new();
   assert(detect);

   int status = uchardet_handle_data(ud, data, len);
   if (status != 0) {
      printf("well, problem happens!\n");
      return ;
   }

   uchardet_data_end(ud);

   const char *charset = uchardet_get_charset(ud);
   printf("answer: [%s]\n", charset);

   uchardet_delete(ud);
}

int detect_bytes_charset_by_libchardet(const char *inbuf) {
   DetectObj *obj = detect_obj_init();
   if (obj == NULL) {
      fprintf (stderr, "Memory Allocation failed\n");
      return CHARDET_MEM_ALLOCATED_FAIL;
   }

   switch (detect(inbuf, &obj)) {
   case CHARDET_OUT_OF_MEMORY :
      fprintf (stderr, "On handle processing, occured out of memory\n");
      detect_obj_free (&obj);
      return CHARDET_OUT_OF_MEMORY;

   case CHARDET_NULL_OBJECT :
      fprintf (stderr,"2st argument of chardet() is must memory allocation with detect_obj_init API\n");
      return CHARDET_NULL_OBJECT;
   }

   printf("encoding: %s, confidence: %f\n", obj->encoding, obj->confidence);
   return 0;
}

void detect_bytes_charset(const char *bytes, int32_t size) {
   UErrorCode status = U_ZERO_ERROR;
   UCharsetDetector* csd = ucsdet_open(&status);
   if(status != U_ZERO_ERROR)
      printf("ucsdet_open 错误: %s\n", u_errorName(status));


   ucsdet_setText(csd, bytes, size, &status);

   int32_t matchesFound = 0;
   const UCharsetMatch **ucms = ucsdet_detectAll(csd, &matchesFound, &status);
   printf("matchesFound: %d\n", matchesFound);
   // const UCharsetMatch *ucm = ucsdet_detect(csd, &status);

   for (int32_t nth = 0; nth < matchesFound; ++nth) {
      UErrorCode status = U_ZERO_ERROR;
      const char *name = ucsdet_getName(ucms[nth], &status);
      printf("charset: [%s]\n", name);
   }

}

// 代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
   char **pin = &inbuf;
   char **pout = &outbuf;

   iconv_t cd = iconv_open(to_charset, from_charset);
   if (cd == 0) return -1;

   memset(outbuf, 0, outlen);

   size_t outbytesleft = outlen;

   if(iconv(cd, pin, &inlen, pout, &outbytesleft) == -1) {
      printf("转码失败, [%s]\n", strerror(errno));
      return -1;
   }

   iconv_close(cd);

   return outlen - outbytesleft;
}

// UNICODE码转为GB2312码
int u2g(char *inbuf, int inlen, char *outbuf, int outlen) { return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen); }

// GB2312码转为UNICODE码
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen) { return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen); }

void dump_cstring_bytes(const char *bytes, size_t sz) {
   printf("[%.*s] ", (int)sz, bytes);
   for (size_t nth = 0; nth < sz; ++nth) {
      printf("%x ", (uint8_t)bytes[nth]);
   }
   putchar('\n');
}

#define OUTLEN 255

int main(int argc, char *argv[]) {
    assert(argc > 1);

    // char *in_utf8 = "正在安装";
    char *in_utf8 = argv[1];

    printf("\e[31m==== ==== ==== ==== 输入\n\e[0m");
    dump_cstring_bytes(in_utf8, strlen(in_utf8));
    detect_bytes_charset(in_utf8, strlen(in_utf8));
    detect_bytes_charset_by_libchardet(in_utf8);
    detect_bytes_charset_by_libuchardet(in_utf8);

    char gb2312_bytes[OUTLEN];

    int nbytes = 0;

    printf("\e[35m---- ---- ---- ---- unicode码转为gb2312码\n\e[0m");
    nbytes = u2g(in_utf8, strlen(in_utf8), gb2312_bytes, OUTLEN);
    dump_cstring_bytes(gb2312_bytes, nbytes);
    detect_bytes_charset(gb2312_bytes, nbytes);
    detect_bytes_charset_by_libchardet(gb2312_bytes);
    detect_bytes_charset_by_libuchardet(gb2312_bytes);

    printf("unicode -> gb2312, [%d], [%s]\n", nbytes, gb2312_bytes);

    printf("\e[34m++++ ++++ ++++ ++++ gb2312码转为unicode码\n\e[0m");
    char utf8_bytes[OUTLEN] = "";
    nbytes = g2u(gb2312_bytes, nbytes, utf8_bytes, OUTLEN);
    dump_cstring_bytes(utf8_bytes, nbytes);

    printf("gb2312 -> unicode, [%d], [%s]\n", nbytes, utf8_bytes);
}
