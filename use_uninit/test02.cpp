/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test02.cpp
 * @brief USE_UNINIT 测试
 * @author 王彩云 (wangcaiyun@softsafe-tech.com)
 */

// #include <cstdlib>
extern "C" {
extern void *malloc(unsigned int num_bytes);
}

struct A {
  int a;
  int *b;
};

void setVal(struct A *call) { call->a = 0; }

int use(struct A *call) { return *(call->b); }

void test01() {
  struct A *ac;
  ac = (struct A *)malloc(sizeof(struct A));
  setVal(ac);
  use(ac);  // #defect#USE_UNINIT
}

void test01s() {
  struct A *ac;
  ac = (struct A *)malloc(sizeof(struct A));
  setVal(ac);
  int x;
  ac->b = &x;
  use(ac);  // #defect#USE_UNINIT
}

int *read(struct A *call) { return call->b; }
void test011s() {
  struct A *ac;
  ac = (struct A *)malloc(sizeof(struct A));
  setVal(ac);
  int x;
  ac->b = &x;
  read(ac);  // #nodefect#USE_UNINIT
}

int use(int *x) { return *x + 2; }

void test02(int c) {
  int *x;
  if (c) x = &c;
  use(x);  // #defect#USE_UNINIT
}
