/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test3.cpp
 * @brief STRING_BUFFER_OVERFLOW 测试
 * @author Wang Caiyun ()
 */

#define MAX_PROP_CFG_LINE_LEN 512
unsigned int strlen(const char* str);
extern "C" {
char* strcpy(char* strDestination, const char* strSource);
char* StrCat(char* strDestination, const char* strSource);

char* strncpy(char* dest, const char* src, unsigned int n);
}

#define pal_strlen strlen
#define pal_strcpy strcpy

void test() {
  char** buf10_1;
  char buf10_2[255];
  char buf100[100];

  if (strlen(buf10_1[4]) <= 255)
    strcpy(buf10_2, buf10_1[4]);
  else
    strncpy(buf10_2, buf10_1[4], 254);
}

void test1() {
  char** buf10_1;
  char buf10_2[255];
  char buf100[100];

  if (pal_strlen(buf10_1[4]) <= 255)
    pal_strcpy(buf10_2, buf10_1[4]);
  else
    strncpy(buf10_2, buf10_1[4], 254);
}

#define pal_strlen1(x) strlen(x)
#define pal_strcpy1(x, y) strcpy(x, y)

void test2() {
  char** buf10_1;
  char buf10_2[255];
  char buf100[100];

  if (pal_strlen1(buf10_1[4]) <= 255)
    pal_strcpy1(buf10_2, buf10_1[4]);
  else
    strncpy(buf10_2, buf10_1[4], 254);
}

#define LEN_MAX 255

void test3() {
  char** buf10_1;
  char buf10_2[255];
  char buf100[100];

  if (buf10_1 == 0) {
    return;
  }

  if (pal_strlen1(buf10_1[4]) <= LEN_MAX)
    pal_strcpy1(buf10_2, buf10_1[4]);
  else
    strncpy(buf10_2, buf10_1[4], 254);
}

void test4(char* line, char* line_tmp) {
  if (strlen(line) > (MAX_PROP_CFG_LINE_LEN - 1)) {
    return;
  }
  strcpy(line_tmp, line);
}

void test5(char* line) {
  char line_tmp[512];
  if (strlen(line) > (MAX_PROP_CFG_LINE_LEN - 1)) {
    return;
  }
  strcpy(line_tmp, line);
}

struct A {
  int x;
  int y;
};

void test6(A* line) {
  A line_tmp[512];
  if (strlen((char*)line) > (MAX_PROP_CFG_LINE_LEN - 1)) {
    return;
  }
  strcpy((char*)line_tmp, (char*)line);
}
