/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test03.c
 * @brief USE_UNTERMINATED_STRING 测试
 * @author Wang caiyun
 */

#include "header.h"

void test(void) {
  char c_str[3] = "abc";
  printf("%s\n", c_str);
}

enum { STR_SIZE = 32 };
unsigned long long test2(const char *source) {
  char c_str[STR_SIZE];
  unsigned long long ret = 0;
  if (source) {
    c_str[sizeof(c_str) - 1] = '\0';

    strncpy(c_str, source, sizeof(c_str));
    ret = strlen(c_str);
  } else {
  }
  return ret;
}

void myPrintf(char *x) { printf("%s\n", x); }

void test01(void) {
  char c_str[3] = "abc";
  myPrintf(c_str);
}

int myRecv(int s, void *buf, int len, int flags) {
  return recv(s, buf, len, flags);
}

void test16(int s, char *p) {
  myRecv(s, p, 30, 1);

  if (*p) {
    strlen(p - 3);
  }
}

char *myStrncpy(char *dest, const char *src, unsigned long long num) {
  return strncpy(dest, src, num);
}

void testa1(void) {
  char src[] = "welcome home";
  char dest[13];
  char *p = dest;
  src[2] = 0;
  src[2] = 1;
  myStrncpy(p, src, 6);
  strlen(p);
}

unsigned long long myStrlen(const char *str) { return strlen(str); }

void test17(int s, char *p) {
  recv(s, p, 30, 1);

  if (*p) {
    myStrlen(p - 3);
  }
}
