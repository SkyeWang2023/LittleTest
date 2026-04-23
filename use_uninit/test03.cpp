/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test02.cpp
 * @brief USE_UNINIT 测试
 * @author 王彩云 (wangcaiyun@softsafe-tech.com)
 */

// #include <string.h>

// #include <cstring>
// #include <iostream>
extern "C" {
extern void *memset(void *s, int c, unsigned long n);
extern void *memcpy(void *dest, const void *src, unsigned long n);
extern char *gets(char *str);
extern char *strcpy(char *dest, const char *src);
}

// 调用系统函数进行初始化
void test01() {
  int x[4];
  memset(x, 0, sizeof(x));  /
  int b = x[1];             /
}

void test02() {
  int x[4];
  memset(&x[1], 0, sizeof(int) * 3);  /
  int b = x[0];                       /
}

char test03() {
  char a[10];
  char b[10] = {};
  memcpy(a, b, sizeof(b));  /
  return a[0];              /
}

char test04() {
  char str1[20];
  gets(str1);      /
  return str1[2];  /
}

char test05() {
  char str1[20];
  strcpy(str1, "helloworld");  / // 应该是std::strcpy()
  return str1[19];             /
}

void test06() {
  int a, b;
  memcpy(&a, &b, 10);  /
  int x = a;           /
}
