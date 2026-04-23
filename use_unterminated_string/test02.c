/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test02.c
 * @brief USE_UNTERMINATED_STRING 测试
 * @author Wang caiyun
 */

#include "header.h"

char* string_null_example() {
  char name[1024];
  read(0, name, 1024);
  strlen(name);  // #defect#USE_UNTERMINATED_STRING
}

struct in_ifaddr {
  char ifa_label[200];
};

struct in_device {
  struct in_ifaddr* ifa_list;
};

void test(struct in_device* in_dev) {
  struct in_ifaddr* ifa;
  char* p;
  char old[100];
  memcpy(old, ifa->ifa_label, 100);
  strlen(old);  // #defect#USE_UNTERMINATED_STRING
}

void test01a(char* p, char* q) {
  memcpy(p, q, 100);
  strlen(p);  // #nodefect#USE_UNTERMINATED_STRING
}

void test02a(char* p) {
  char ary[100];
  memcpy(p, ary, 100);
  strlen(p);  // #nodefect#USE_UNTERMINATED_STRING
}

void test03a(char* p) {
  char ary[100];
  char* q = ary;
  memcpy(p, q, 100);
  strlen(p);  // #nodefect#USE_UNTERMINATED_STRING
}

void test04a(char* p) {
  char ary[100];
  char aryq[100];
  memcpy(aryq, ary, 10);
  strlen(aryq);  // #defect#USE_UNTERMINATED_STRING
}

void test05a(char* p) {
  char ary[100];
  char aryq[200];
  memcpy(aryq, ary, 10);
  strlen(aryq);  // #defect#USE_UNTERMINATED_STRING
}

void test06a(struct in_device* in_dev) {
  struct in_ifaddr* ifa;
  char* p;

  char old[100];
  char* yytext = "abod\"o\"hoasd";
  char* k = strchr(yytext, '\"') + 1;
  char* e = strchr(k, '\"');
  *e = '\0';
  char* x = (char*)memcpy(old, ifa->ifa_label, 200);
  strlen(x);  // #nodefect#USE_UNTERMINATED_STRING
}

void test07a(char* path) {
  char* symbolic_link = 0;

  char buf[201];
  readlink(symbolic_link, buf, 200);
  strcmp(path, buf);  // #defect#USE_UNTERMINATED_STRING
}

void test01(char* path) {
  char* symbolic_link = 0;

  char buf[201];
  memset(buf, 0, 201);
  readlink(symbolic_link, buf, 200);
  strcmp(path, buf);
}

void test01b(char* path) {
  char* symbolic_link = 0;

  char buf[201];
  __builtin_memset(buf, 0, 201);
  readlink(symbolic_link, buf, 200);
  strcmp(path, buf);  // #nodefect#USE_UNTERMINATED_STRING
}

void test02(char* path) {
  char* symbolic_link = 0;

  char buf[201];
  memset(buf, 1, 201);
  readlink(symbolic_link, buf, 200);
  strcmp(path, buf);  // #defect#USE_UNTERMINATED_STRING
}

void setArray(char* buf) {
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = 0;
}

void test03(int s) {
  char buf[3];
  setArray(buf);
  recv(s, buf, 2, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test04(int s) {
  char buf[201];
  memset(buf, 0, 201);
  recv(s, buf, 200, 1);
  strlen(buf);
}

void test05(int s) {
  char buf[201];
  memset(buf, 1, 201);
  recv(s, buf, 200, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test06(int s) {
  char buf[201];
  memset(buf, 0, 201);
  recv(s, buf, 300, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test07(char* path) {
  char* symbolic_link = 0;

  char buf[201];
  readlink(symbolic_link, buf, 200);
  strcmp(path, buf);  // #defect#USE_UNTERMINATED_STRING
}
void test07k(char* path) {
  char* symbolic_link = 0;
  char buf[201];
  memset(buf, 0, 201);
  readlink(symbolic_link, buf, 400);
  strcmp(path, buf);  // #defect#USE_UNTERMINATED_STRING
}

void test08(int s) {
  char buf[201];
  int k = 0;
  memset(buf, k, 201);
  recv(s, buf, 20, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test09(int s) {
  char buf[201];
  memset(buf, 0, 10);
  recv(s, buf, 200, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test10(int s, int len) {
  char buf[201];
  memset(buf, 0, len);
  recv(s, buf, 200, 1);
  strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
}

void test11(int s, int len) {
  char buf[201];
  memset(buf, 0, 300);
  recv(s, buf, 299, 1);
  strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
}

void test12(int s, int len, char* buf) {
  memset(buf, 0, 300);
  recv(s, buf, 299, 1);
  strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
}

void test13(int s) {
  char p[100] = {'a', 'b'};
  recv(s, p, 299, 1);
  strlen(p);  // #defect#USE_UNTERMINATED_STRING
}

void test14(int s) {
  char p[100] = {'a', 'b'};
  recv(s, p, 30, 1);
  strlen(p);  // #nodefect#USE_UNTERMINATED_STRING
}

void test15(int s, int len) {
  char p[100] = {'a', 'b'};
  recv(s, p, len, 1);
  strlen(p);  // #nodefect#USE_UNTERMINATED_STRING
}

void test16(int s, int len, char* buf) {
  memset(buf, 0, 300);
  recv(s, buf, 300, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test13a(int s, int len, char* buf) {
  memset(buf, 0, 300);
  buf[3] = '3';
  recv(s, buf, 299, 1);
  strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
}

void test14a(int s, int len) {
  char buf[4] = {'u', 'o', 'i', '2'};
  buf[3] = 0;
  recv(s, buf, 299, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test15a(int s, int len, char* buf) {
  static const char dirletter[4] = {'u', 'o', 'i', '2'};
  strncpy(buf, dirletter, 4);
  recv(s, buf, 299, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test16a(int s, int len, char* buf) {
  char* dirletter = "qwetr";
  recv(s, dirletter, 299, 1);
  strlen(dirletter);  // #defect#USE_UNTERMINATED_STRING
}

void test17(int s, int len, char* buf) {
  const char* dirletter = "qwetr";
  recv(s, dirletter, 299, 1);
  strlen(dirletter);  // #defect#USE_UNTERMINATED_STRING
}

void test18(int s, int len) {
  char buf[4] = {'u', 'o', 'i', '2'};
  buf[3] = 0;
  recv(s, buf, 2, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test19(int s, int len, char* buf) {
  static const char dirletter[4] = {'u', 'o', 'i', '2'};
  strncpy(buf, dirletter, 4);
  recv(s, buf, 2, 1);
  strlen(buf);  // #defect#USE_UNTERMINATED_STRING
}

void test20(int s, int len, char* buf) {
  char* dirletter = "qwetr";
  recv(s, dirletter, 2, 1);
  strlen(dirletter);  // #defect#USE_UNTERMINATED_STRING
}

void test21(int s, int len, char* buf) {
  const char* dirletter = "qwetr";
  recv(s, dirletter, 2, 1);
  strlen(dirletter);  // #defect#USE_UNTERMINATED_STRING
}
