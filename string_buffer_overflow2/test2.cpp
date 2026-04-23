/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test2.cpp
 * @brief STRING_BUFFER_OVERFLOW 测试
 * @author Wang Caiyun ()
 */

extern "C" {
char* strcpy(char* strDestination, const wchar_t* strSource);
}

#define __T(x) L##x
#define _T(x) __T(x)

void test13() {
  char buf10_1[10];

  strcpy(buf10_1, _T("tyouwroqwhotwieroh"));
}

char* StrCat(char* strDestination, const char* strSource) {
  strSource++;
  return 0;
}

void test4a(char* a) {
  StrCat("::", a);
}
