/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test04.c
 * @brief USE_UNTERMINATED_STRING 测试
 * @author Wang caiyun
 */

#include "header.h"

char gNonNullTerminatedStr[12] = "Welcome home";
wchar_t gNonNullTerminatedWStr[5] =  {0x0074,0x72a2, 0x4f53,0x4e2d,0x6587};

char gNullTerminatedStr[] = "Welcome home";
wchar_t gNullTerminatedWStr[] = {0x0074,0x72a2, 0x4f53,0x4e2d,0x6587};
void bc1(void) {
  char str[12] = "welcome home";
  strlen(str);             // required
}

void bc1a(void) {

  char str[12] = "welcome home";
  char buf[12];
  sprintf(buf, "%s", str); // required

  strlen(buf); // #fn_issue
}

void bc2(void) {

  char str[12] = "Welcome home";
  printf("%s", str); // required
}

void bc3(void) {

  unsigned long long size = 10;
  char *str = (char *)malloc(sizeof(char) * size);

  strlen(str); // #fn_issue
}

void bc3c(void) {

  unsigned long long size = 10;
  char *str = (char *)malloc(sizeof(char) * size);

  char *tmp = realloc(str, size / 2);

  tmp = calloc(size / 2, size);

  strlen(tmp); // #fn_issue
}

void bc3d(void) {

   unsigned long long size = 10;

  strlen(calloc(size / 2, size)); // #fn_issue
}

void bc4(void) {
  char src[] = "welcome home";
  char dest[12];
  dest[11] = '\0';
  strncpy(dest, src, 3);
  strlen(dest); // required
}

void bc5(const char *src, const int arr_size) {
  char dest[arr_size];
  dest[arr_size - 1] = '\0';
  if (src) {
    strncpy(dest, src, arr_size);
    strlen(dest); // not required
  }
}

void bc6(void) {

 

  strlen(gNonNullTerminatedStr); // #fn_issue

  wcslen(gNonNullTerminatedWStr); // #fn_issue
}

void bc7(void) {
  char src[12] = "welcome home";
  char dest[13];
  strcpy(dest, src); // required
}

void bc8(void) {
  char src[] = "welcome home";
  char dest[12];
  for (int i; i < 12; i++) {
    dest[i] = src[i];
  }
  strlen(dest); // #fn_issue

  printf("%s\n", dest); // #fn_issue
}

extern int read(int, char *, int);
char *string_null_example() {
  char name[1024];
  read(0, name, 1024);
  strlen(name); // required
}
void gc1(void) {
  char str[] = "welcome home";
  char buf[100];
  sprintf(buf, "%s", str); // #no_issue
}

void gc2(void) {
  char str[] = "Welcome home";
  printf("%s", str); // #no_issue
}

void gc3(unsigned long long str_size) {
  char *str = (char *)malloc(str_size);
  str[str_size - 1] = '\0';
  unsigned long long size = strlen(str); // #no_issue
  free(str);
}

void gc4(void) {
  char src[] = "welcome home";
  char dest[13];
  strncpy(dest, src, 13);
  strlen(dest); // #no_issue
}

void gc4a(void) {
  char src[] = "welcome home";
  char dest[13];
  strncpy(dest, src, 4);
  strlen(dest); // required
}

void gc5(const char *src, unsigned long long arr_size) {
  char dest[arr_size];
  strncpy_s(dest, sizeof(dest), src, arr_size); // #no_issue
  if (src) {
    if (strlen(src) < arr_size) {
      strcpy(dest, src); // #no_issue
      strlen(dest);      // #no_issue
    } else {
      exit(1);
    }
  } else {
    exit(1);
  }
}

void gc6(void) {
  char src[12] = "welcome\0home";
  char dest[13];
  strlen(src);       // #no_issue
  strcpy(dest, src); // #no_issue
}

void gc7(void) {
  strlen(gNullTerminatedStr);  // #no_issue
  wcslen(gNullTerminatedWStr); // #no_issue
}


void test03(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
    sprintf(hdlen, ", H%c=%u", dirletter[x],100);  // not required
  
}

void test04(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
    sprintf(hdlen, ", %p", dirletter);  // not required
  
}

void test05(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
    sprintf(hdlen, "%s=%s", &dirletter[x],dirletter);  // required
  
}


void test06(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];

  // not required
  sprintf(hdlen, "%s=%s", &dirletter[x], &dirletter[x]);
}


void test07(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p = dirletter;
  
    sprintf(hdlen, "%s", p+1);  // required
  
}


void test08(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p[10];
  p[1]  = dirletter;
  
    sprintf(hdlen, "%s", p[1]);  // required
  
}


void test09(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p = dirletter;

  sprintf(hdlen, "%s", &p[1]);  // not required
}

void test10(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  void* p = dirletter;
  
    sprintf(hdlen, "%s", (char*)(p+1));  // required
  
}

void test11(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%s";
  
    sprintf(hdlen, format, dirletter);  // required
  
}

void test12(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%p";
  
    sprintf(hdlen, format, dirletter);  // required
  
}

void test13(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%p,%c";
  
    sprintf(hdlen, format, dirletter,dirletter[0]);  // required 
  
}

void test14(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  void* p = dirletter;
  
    sprintf(hdlen, "%n", p);  // required
  
}

void test15(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p = dirletter;
  
    sprintf(hdlen, "%c", p[1]);  // not required
  
}

void test16(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%s,%s";
  
    sprintf(hdlen, format, dirletter,dirletter);  // required 
  
}

void test17(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%c,%s";
  
    sprintf(hdlen, format, dirletter[0],dirletter);  // required 
  
}

void test18(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p = dirletter;
  
  char* format = "%s,%s";
  
    sprintf(hdlen, format, dirletter,p);  // required 
  
}

void test19(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%c";
  
    sprintf(hdlen, format, dirletter[0]);  // not required 
  
}

void test20(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%s";
  
    sprintf(hdlen, format, &dirletter[0]);  // not required 
  
}

void test21(int s, void* p,int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* format = "%p";
  
    sprintf(hdlen, format, dirletter);  // required 
  
}

void test22(int s, int x) {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  char* p = dirletter;
  
  char* format = "%p,%s";
  
    sprintf(hdlen, format, dirletter,p);  // required 
  
}

typedef struct myS{
  char* p;
}myS;

void test23() {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  
  myS s;
  
  s.p = dirletter;
  
  char* format = "%p,%s";
  
  sprintf(hdlen, format, s.p,dirletter);  // required 
  
}


void test24() {
  static const char	dirletter[4] = {'u', 'o', 'i', 'n'};
  char			hdlen[20];
  char* p = dirletter;
  
    sprintf(hdlen, "%s=%s", p,dirletter);  // required
  
}

void test25(char**p) {
  char c_str[3] = "abc";
  *p = c_str;
  printf("%s\n", *p);  // required
}
