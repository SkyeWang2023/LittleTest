/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test02.cpp
 * @brief USE_UNINIT 测试
 * @author 王彩云 (wangcaiyun@softsafe-tech.com)
 */

extern "C" {
extern void *malloc(unsigned int num_bytes);
}

extern void f(int a);

int test1() {
  int a;
  f(a);  // #defect#USE_UNINIT
  return a;
}

int test11() {
  int a;
  f((int)a);  // #defect#USE_UNINIT
  return a;
}

extern void faddr(int *a);
int test2() {
  int a;
  faddr(&a);  // #nodefect#USE_UNINIT
  return a;   // #nodefect#USE_UNINIT
}

int *test3() {
  int *a;
  faddr(a);  // #defect#USE_UNINIT
  return a;
}

class A {
 public:
  int a;
  int *b;
};

int *test4() {
  A ca;
  faddr(ca.b);  // #defect#USE_UNINIT
  return ca.b;
}

int test5() {
  A ca;
  faddr(&ca.a);
  return ca.a;
}

int test6() {
  A ca;
  f(ca.a);  // #defect#USE_UNINIT
  return ca.a;
}

int test7() {
  A ca;
  f(*(ca.b));      // #defect#USE_UNINIT
  int x = ca.a;    // #defect#USE_UNINIT
  return *(ca.b);  // #nodefect#USE_UNINIT
}

typedef void (*block128_f)(const unsigned char arr[16], const void *key);

typedef union {
  unsigned char c[16];
} Block_c;

typedef struct context {
  block128_f encrypt;
  void *keyenc;
} CONTEXT;

void callUnimplemented(int *ptr);

void test(CONTEXT *ctx) {
  int *p;

  callUnimplemented(p);  // #defect#USE_UNINIT

  Block_c tmp_c;
  ctx->encrypt(tmp_c.c, ctx->keyenc);  // #nodefect#USE_UNINIT
}

void *testPtr(unsigned long len);

void test() {
  int *p;
  p = (int *)testPtr(sizeof(*p));  // #nodefect#USE_UNINIT
}

void test03() {
  int *p;

  int *p2;
  p2 = (int *)testPtr(sizeof(*p));  // #nodefect#USE_UNINIT
}

void test04() {
  int *p;

  *p = sizeof(*p);  // #defect#USE_UNINIT
}
