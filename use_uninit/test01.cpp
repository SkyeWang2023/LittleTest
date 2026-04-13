/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test01.cpp
 * @brief USE_UNINIT 测试
 * @author 王彩云 (wangcaiyun@softsafe-tech.com)
 */

int test01(int c) {
  int x;
  if (c)
    return c;
  else
    return x;  // required
}

void test03() {
  int x[4];
  int b = x[1];  // required
}

void test04() {
  int x;
  int y = x;  // required
}

void test05() {
  int x[4];
  for (int i = 0; i < 4; i++) {
    int b = x[i];  // required
  }
}

int test06(int x) {
  int a, b;
  switch (x) {
    case 1:
    case 2:
    case 3:
      a = b;  // required
      break;
    case 4:
      int c;
      return c;  // required
  }
  return a;  // required
}

struct CC {
  int a;
  int b;
  int q;
};
void test07() {
  CC c;
  c.b = c.a;  // required
}

union UU {
  int a;
  char b;
};

char test08() {
  UU u;
  u.a = 4;
  return u.b;  // not required
}

void test10() {
  int a;
  int *p = &a;  // not required
}

int test11() {
  int a;
  if (a) {  // required
    return a + 1;
  } else {
    return 2;
  }
}

void test12() {
  int a;
  int b = 1;
  a = a + 1 + a + 2 + a + b;  // required
}

int ga;
int test13() { return ga + 4; }  // not required

int test14(int c) {
  int x;
  if (c) {
    x = 5;
    return x;
  } else {
    return x + 6;  // required
  }
}

int test15(int c) {
  int x;
  if (c) {
    x = 5;
  }
  return x + 6;  // required
}

void test16() {
  int x[4] = {1, 2};
  for (int i = 0; i < 4; i++) {
    int b = x[i];  // not required
  }
}

void test17() {
  int x[4];
  x[0] = 1;
  x[2] = 2;
  for (int i = 0; i < 4; i++) {
    int b = x[i];  // #fndefect#USE_UNINIT  SAST-738
  }
}

void test171() {
  int x[4];
  x[0] = 1;
  x[1] = 2;
  x[2] = 2;
  x[3] = 4;
  for (int i = 0; i < 4; i++) {
    int b = x[i];  // not required
  }
}

int test18() {
  CC c;
  c.b = 1;
  return c.a;  // required
}

int test19() {
  int *p;
  *p = 1;  // required
  return 0;
}

struct DD {
  int q;
  CC c;
};

void test20(int x) {
  DD d;
  d.c.a = d.c.a + 1;  // required
}

void test21(int x) {
  DD d;
  d.c.q = 1;
  d.c.a = d.c.b + 1;  // required
}

void test22(int x) {
  DD d;
  d.c.q = 1;
  d.c.a = d.c.q + 1;  // not required
}

void test23(int x) {
  DD d;
  d.c = {1, 2};
  x = d.q;  // required
}

int test24(int x) {
  DD d;
  d.c = {1, 2};
  d.c.a = d.c.a + 1;  // not required
  d.c.q = 1;          // not required
  d.c.a = d.c.b + 1;  // not required
  return d.c.b;       // not required
}

int test25() {
  int x[10];
  x[0] = 1;
  x[3] = x[3] + 1;  // #fndefect#USE_UNINIT  SAST-1368
  return x[4];      // #fndefect#USE_UNINIT  SAST-1368
}

int test26() {
  int x[10][20];
  return x[0][1];  // required
}

int test27() {
  int x[10];
  x[3] = x[3] + 1;  // required
  return 0;
}

int test28() {
  int i;
  i *= 9;  // required //use i first
  return 0;
}

int globalInt;
void test() {
  int j = globalInt;  // not required
}

class Aben {
 public:
  int a;
  int b;
};

class Bben : public Aben {
 public:
  int c;
};

void fun2() {
  Bben y;
  Aben *x = (Aben *)&y;
  x->a = 0;
  x->b = 0;
  int n = y.a;  // not required
  n = y.b;      // not required
  n = y.c;      // required
}

void fun3() {
  Aben y;
  Bben *x = (Bben *)&y;
  x->a = 0;
  int n = y.a;  // not required
  n = y.b;      // required
}

class Test6 {
 public:
  char buf2[0];
  int var1;
  int var2;
  char buf[0];
};

void test6() {
  Test6 tg6;
  tg6.var1 = 0;
  tg6.var2 = 0;
  char x = tg6.buf2[0];
}

class Test7 {
 public:
  char buf2[0];
  int var1;
  int var2;
  char buf[0];
};

void test7() {
  Test7 tg7;
  tg7.var1 = 0;
  char x = tg7.buf2[0];  // required
}

class Test8 {
 public:
  char buf2[0];

  char buf[0];
};

void test8() {
  Test8 tg8;
  char x = tg8.buf2[0];  // required
}

class Test9 {
 public:
  char buf[0];
};

void test9() {
  Test9 tg9;
  char x = tg9.buf[0];  // required
}

struct rbsp {
  char *buf;
  int size;
  int pos;
};

void fun9(char *buf) {
  struct rbsp sps;
  int pos;
  int ret;

  sps.buf = buf + 5; /* Skip NAL header */

  auto x = sps.buf[0];  // not required
}

extern "C" {
void *malloc(unsigned long long size);
}
void f(char *);

typedef struct mca_common_ompio_access_array_t {
  char *offsets;
  int *lens;
  int count;
} mca_common_ompio_access_array_t;

void f(char *);
typedef void (*def)(char *);

void test(mca_common_ompio_access_array_t **others_req_ptr, int i) {
  mca_common_ompio_access_array_t *others_req = 0;
  *others_req_ptr = (mca_common_ompio_access_array_t *)malloc(100);
  others_req = *others_req_ptr;
  others_req[i].count = 0;
  def fpr;
  fpr = f;
  fpr(others_req[i].offsets);  // required
}
