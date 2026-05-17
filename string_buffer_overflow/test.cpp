/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test.cpp
 * @brief STRING_BUFFER_OVERFLOW 测试
 * @author Wang Caiyun ()
 */
// #include <cstring>
// #include <tchar.h>

// clang-format off

typedef void *va_list;
extern "C"{
  void *malloc(unsigned long long size);
char* strcpy(char* strDestination, const char* strSource);
 char* StrCat(char* strDestination, const char* strSource);
 int sprintf(char *str, const char *format, ...);
 int vsprintf(char *str, const char *format, va_list arg);
 char* strchr(const char* s, int c);
 int *fopen(const char *filename, const char *mode);
 int strcmp(const char *str1, const char *str2);
}

typedef char CHAR;

class CharA {
 public:
  char m;
};

void test() {
  char buf10_1[10];
  char buf10_2[10];
  char buf100[100];

  strcpy(buf10_1, buf10_2);

  strcpy(buf10_1, buf10_1);

  CharA bufCls[10];
  strcpy(buf10_1, (char*)bufCls);

  strcpy(buf10_1, buf100);

  int bufInt[10];
  strcpy(buf10_1, (char*)bufInt);
}

void test01() {
  char buf10_1[10];

  int bufInt[10];
  strcpy(buf10_1, (char*)bufInt);
}

void test1() {
  char buf10_1[10];
  char buf10_2[10];
  char buf100[100];

  char* p = buf10_2;
  strcpy(buf10_1, p);
}

void test2() {
  char buf10_1[10];
  char buf10_2[10];
  char buf100[100];

  char* p = buf100;
  strcpy(buf10_1, p);
}

void test3() {
  char buf10_1[10];
  char buf10_2[100] = {"abc"};

  strcpy(buf10_1, buf10_2);
}

void test4() {
  char buf10_1[10];
  char buf10_2[] = {"abc"};

  strcpy(buf10_1, buf10_2);
}

void test5() {
  char buf10_1[10];
  char buf10_2[] = {"abcytiwefiufiwuhifugwaeifugai"};

  strcpy(buf10_1, buf10_2);
}

void test7() {
  char buf10_1[10];

  strcpy(buf10_1, "tyouwroqwhotwieroh");

  strcpy(buf10_1, "er");
}

void test8() {
  char buf10_1[10];

  char* p = "tyouwroqwhotwieroh";

  strcpy(buf10_1, p);

  char* p1 = "abc";
  strcpy(buf10_1, p1);
}

char gbuf100[100];
char gbuf10[10];
void test9() {
  char buf10_1[10];
  strcpy(buf10_1, gbuf100);
}

void test10() {
  char buf10_1[10];
  strcpy(buf10_1, gbuf10);
}

void test11() {
  char buf10_1[10];
  CHAR buf100[100];
  strcpy(buf10_1, buf100);
}

void test12() {
  char buf10_1[10];

  char* p = "tyouwroqwhotwieroh";

  char* q = p;

  strcpy(buf10_1, q);

}

void test14() {
  char buf10_1[10];
  char buf100[100];
  char* t = buf10_1;
  char* s = buf100;

  strcpy(t, s);

}

class MultiElem{
  public:
    int x;
    int y;
    char buf[100];
};

void test15(){
  char buf[10];
  MultiElem bufM[10];
  strcpy(buf, (char*)bufM);

}

enum ENUM{
  one,
  two,
  three
};

void test16(){
  char buf[10];
  ENUM bufM[10];
  strcpy(buf, (char*)bufM);
}

void test17(){
  char buf[10];
  MultiElem a;
  strcpy(buf, a.buf);

}

void test19() {
  char buf10_1[10];
  char* t = buf10_1;
  char* p = "riqgiqewguhoqwei gioiwehoqwihe[oeqwi";

  char* q = p;

  strcpy(t, q);

}

void test20(){
  char buf[10];
  char buf1[100] = {'a','a', 'b'};
  strcpy(buf, buf1);
}

void test21(){
  char buf[2];
  int buf1[100] = {1};
  strcpy(buf, (char*)buf1);
}

void test22(){
  char buf[4];
  int buf1[100] = {1};
  strcpy(buf, (char*)buf1);
}

void test23() {
  char buf10_1[10];
  char buf10_2[100] = {"abc"};

  for(int i = 3; i < 20; i++){
    buf10_2[i] = 'a';
  }


  strcpy(buf10_1, buf10_2);
}

void test25() {
  char buf10_1[10];
  char buf10_2[10];

  strcpy(buf10_1 + 2, buf10_2);
}

void testC1() {
  char target[2][10];
  char src[100];
  strcpy(target[0], src);
}

class A {
 public:
  char buf[10];
};

void testC2() {
  A a;
  char(A::*mPtr)[10] = &A::buf;
  char src[100];
  strcpy(a.*mPtr, src);
}

void testC3() {
  A* a;
  char src[100];
  strcpy(a->buf, src);
}

void testC5() {
  char target[10];
  char* src[10];
  char buf[100];
  src[0] = buf;
  strcpy(target, src[0]);
}

void test4a(char* a){
 StrCat("::",a);
}

void test4b(char *format){
  char dst[4] = {'\0'};
  sprintf(dst, format);

  va_list args;
  vsprintf(dst, format, args);
}

void test4c() {
  char ary[30];
  char *pkey_ptr = strchr(ary, '/');
  strcpy(ary, pkey_ptr);
}

void test45() {
  char* ary[] = {"abc", "efgg"};
  char aryDes[100];
  strcpy(aryDes, ary[1]);
}

void test46() {
  char* ary[] = {"abc", "efgg"};
  char aryDes[100];
  char** p = ary;
  strcpy(aryDes, *(p + 1));
}

#define MYSQL_ERRMSG_SIZE 512
#define ER(X) \
  (((X) >= 2000 && (X) <= 2063) ? client_errors[(X)-2000] : client_errors[0])
#define CR_UNSUPPORTED_PARAM_TYPE 2036

const char* client_errors[] = {
    "Unknown MySQL error",
    "Can't create UNIX socket (%d)",
    "Can't connect to local MySQL server through socket '%-.100s' (%d)",
    "Can't connect to MySQL server on '%-.100s' (%d)",
    "Can't create TCP/IP socket (%d)",
};

typedef struct st_mysql_stmt {
  char last_error[MYSQL_ERRMSG_SIZE]; /* error message */
  unsigned int last_errno;
} MYSQL_STMT;

void test47(MYSQL_STMT* stmt) {
  char ary[100];

  strcpy(ary, ER(stmt->last_errno = CR_UNSUPPORTED_PARAM_TYPE));
}

void test48(int flag) {
  char ary[2];

  strcpy(ary, flag ? "a" : "ab");
}

void test57(char *p, unsigned int size) {
  char *s;
  for (;;) {

    strcpy(s, "../../");
    s += 6;
  }
}

int rdl_verify(const char *filename) {
  int *fp;
  char buf[257];
  int i;
  int length;
  static char lastverified[256];
  static int lastresult = -1;

  if (lastresult != -1 && !strcmp(filename, lastverified)) return lastresult;

  fp = fopen(filename, "rb");

  strcpy(lastverified, filename);
}

int test2(const char *filename) {
  int len;
  filename += len;
  static char lastverified[256];

  strcpy(lastverified, filename);
}

int test3(char *filename) {
  int len;
  *filename = 'c';
  static char lastverified[256];

  strcpy(lastverified, filename);
}

int test4(char *filename) {
  char len;
  filename = &len;
  static char lastverified[256];

  strcpy(lastverified, filename);
}

int test5(char *filename) {
  char **len;
  filename = *len;
  static char lastverified[256];

  strcpy(lastverified, filename);
}

struct nf_ct_timeout {
  char data[100];
};

struct nf_conntrack_l4proto {};

struct T {
  char name[100];
  const struct nf_conntrack_l4proto *l4proto;
  struct nf_ct_timeout timeout;
};

char *nla_data(const struct nlattr *nla);

struct net {};

struct nfnl_info {
  struct net *net;
};

struct nlattr {
  unsigned short nla_len;
  unsigned short nla_type;
};

static int ctnl_timeout_parse_policy(void *timeout,
                                     const struct nf_conntrack_l4proto *l4proto,
                                     struct net *net,
                                     const struct nlattr *attr);

void test10(const struct nfnl_info *info, const struct nlattr *const cda[]) {
  const struct nf_conntrack_l4proto *l4proto;
  char *name;
  name = nla_data(cda[10]);

  T *timeout = (T *)malloc(100);

  strcmp(timeout->name, name);
  if (timeout == 0) {
    return;
  }
  int ret;

  ret = ctnl_timeout_parse_policy(&timeout->timeout.data, l4proto, info->net,
                                  cda[10]);


  strcpy(timeout->name, nla_data(cda[10]));
}

// clang-format on
