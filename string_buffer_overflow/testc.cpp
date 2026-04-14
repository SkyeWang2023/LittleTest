/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file testc.cpp
 * @brief STRING_BUFFER_OVERFLOW 测试
 * @author Wang Caiyun ()
 */
// 文件中的case主要为review过程中添加的case
extern "C" {
char* strcpy(char* strDestination, const char* strSource);
unsigned int strlen(const char* str);
unsigned int strnlen(const char* s, unsigned int maxlen);
unsigned int strnlen_s(const char* str, unsigned int strsz);
unsigned int wcslen(const wchar_t* str);
unsigned int wcsnlen_s(const wchar_t* str, unsigned int strsz);
wchar_t* wcscpy(wchar_t* dest, const wchar_t* src);
char* strncpy(char* dest, const char* src, unsigned int n);
void* memcpy(void* dest, const void* src, unsigned int n);
unsigned char* _mbscat(unsigned char* strDestination,
                       const unsigned char* strSource);
char* strcat(char* dest, const char* src);
}

void test1() {
  char buf[100];
  char* d = "abc";

  strcpy(d, buf);
defectSTRING_BUFFER_OVERFLOW}

void test2() {
  char buf[3] = "de";
  char* d = "abc";

  strcpy(d, buf);
nodefectSTRING_BUFFER_OVERFLOW}

void test3() {
  char* p = "abc";
  strcpy(p, "abcde");
defectSTRING_BUFFER_OVERFLOW}

void test4() {
  char* p1 = "abc";
  char* p2 = "abcde";
  strcpy(p1, p2);
defectSTRING_BUFFER_OVERFLOW}

void test5() {
  char arr1[10];
  char arr2[20];

  if (strlen(arr2) < 10) {
    strcpy(arr1, arr2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test6() {
  char arr1[10];
  char arr2[20];

  if (strlen(arr2) < 15) {  // arr2 可能是 10-15 之间，会overflow
    strcpy(arr1, arr2);
defectSTRING_BUFFER_OVERFLOW  }
}

void test7() {
  char arr1[10];
  char* arr2 = "yoafhohfoawetqa";

  if (strlen(arr2) < 10) {
    strcpy(arr1, arr2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test8() {
  char arr1[10];
  char* arr2 = "yoafhohfoasaweta";

  if (strlen(arr2) < 15) {  // arr2 可能是 10-15 之间，会overflow
    strcpy(arr1, arr2);
fndefectSTRING_BUFFER_OVERFLOW //__SAME__  }
}

class AryClass {
 public:
  char buf[100];
};

void test9(AryClass* pCls) {
  char buf10[10];
  strcpy(buf10, pCls->buf);
defectSTRING_BUFFER_OVERFLOW}

void test10() {
  char buf10_1[10];
  char buf10_2[10];

  if (sizeof(buf10_2) < strnlen(buf10_1, 20)) {
    strcpy(buf10_1, buf10_2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test11(void) {
  wchar_t buf[10];
  wchar_t str[] = L"How many wide characters does this string contain?";

  if (sizeof(buf) > wcslen(str)) {
    wcscpy(buf, str);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test12(void) {
  char buf10_1[10];
  char buf10_2[100];

  strncpy(buf10_1, buf10_2, 100);
nodefectSTRING_BUFFER_OVERFLOW}

void test13(void) {
  char buf10_1[10];
  char buf10_2[100];

  memcpy(buf10_1, buf10_2, 20);
nodefectSTRING_BUFFER_OVERFLOW}

void test14(void) {
  unsigned char buf10_1[10];
  unsigned char buf10_2[100];


defectSTRING_BUFFER_OVERFLOW  _mbscat(buf10_1, buf10_2);
}

void test15(void) {
  char buf10_1[10];
  char* buf10_2 = "abc676785879567o5679";

  if (sizeof(buf10_1) > strlen(buf10_2)) {
    strcat(buf10_1, buf10_2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test15a(void) {
  char buf10_1[10];
  char* buf10_2 = "abc676785879567o5679";

  if (strlen(buf10_2) < 50) {
    strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW  }
}

void test15b(void) {
  char buf10_1[10];
  char* buf10_2 = "abc676785879567o5679";

  if (strlen(buf10_2) < 5) {
    strcat(buf10_1, buf10_2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test16(void) {
  char buf10_1[10];
  char* buf10_2 = "abc676785879567o5679";

  strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW
  strcat(buf10_1, "abc");
nodefectSTRING_BUFFER_OVERFLOW}

void test17() {
  char buf10_1[10] = "abcded";
  char buf10_2[100];
  char buf10_3[100] = "abc";

  strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW  strcat(buf10_1, buf10_3);
defectSTRING_BUFFER_OVERFLOW}

void test171() {
  char buf10_1[10] = "abcded";
  char buf10_3[100] = "abc";

  strcat(buf10_1, buf10_3);
defectSTRING_BUFFER_OVERFLOW}

void test18() {
  char buf10_1[10] = "abcded";
  char buf10_2[100] = "hohgowegoawenbgoaw";

  strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW
  if (strlen(buf10_2) < 10) {
    strcat(buf10_1, buf10_2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test19() {
  char buf10_1[10] = "abcded";
  char buf10_2[100] = "hohgowegoawenbgoaw";

  if (strlen(buf10_2) < 20) {
    strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW  }
}

void test20() {
  char buf10_1[10] = "abcded";
  char* buf10_2 = "hohgowegoawenbgoaw";

  if (strlen(buf10_2) < 10) {
    strcat(buf10_1, buf10_2);
nodefectSTRING_BUFFER_OVERFLOW  }
}

void test22() {
  char buf10_1[10] = "abcded";
  char* buf10_2 = "hohgowegoawenbgoaw";

  if (strlen(buf10_2) < 20) {
    strcat(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW  }
}

void test23() {
  char buf10_1[10] = "abcded";
  char* buf10_2 = "hohgowegoawenbgoaw";

  strcpy(buf10_1, buf10_2);
defectSTRING_BUFFER_OVERFLOW}

void fu() {
  char log_index_path[100];
  char src[100];
  src[99] = 0;
  if (src[99] == 0) {
    strcpy(log_index_path, src + 6);
defectSTRING_BUFFER_OVERFLOW  }
}

void fu1() {
  char dest[100];
  char src[100];
  src[99] = 0;
  if (*(src + 99) == 2) {
    strcpy(dest, src + 6);
defectSTRING_BUFFER_OVERFLOW  }
}

void fu2() {
  char log_index_path[100];
  char src[100];
  if (*(src + 99) == 0) {
    strcpy(log_index_path, src + 6);
defectSTRING_BUFFER_OVERFLOW  }
}

void fu3() {
  char log_index_path[100];
  char src[100];
  src[99] = 0;
  if (strlen(src + 6) < 30) {

nodefectSTRING_BUFFER_OVERFLOW    strcpy(log_index_path, src + 6);
  }
}

int test1(char* src, int len) {
  char buf[100];
  if (strlen(buf) > strlen(src)) {

nodefectSTRING_BUFFER_OVERFLOW    strcpy(buf, src);
  }
}

char* nla_data(char* p);
unsigned long long nla_len(char* p);

int test12(char* src, unsigned long long len, char* param_data) {
  char buf[32];
  len = strnlen(nla_data(param_data), nla_len(param_data));
  if (len == nla_len(param_data) || len >= 32) return -1;

nodefectSTRING_BUFFER_OVERFLOW  strcpy(buf, nla_data(param_data));
}

class AQ {
 public:
  void* x;
  int y;
  char buf[10];
};

int test75(char* src, unsigned long long len, char* param_data,
           unsigned long long len1) {
  AQ a;

nodefectSTRING_BUFFER_OVERFLOW  strcpy((char*)((unsigned long)a.x + a.y), src);
}
