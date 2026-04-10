/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test3.c
 * @brief RESOURCE_LEAK 测试
 * @author 史林霞 (shilinxia@softsafe-tech.com)
 */
// 需要使用头文件识别成系统库函数
#include <stdlib.h>

#define HIGH 3
#define HIGHEST 6
#define track track1(), track2

int fu2(int level) { return level >= HIGHEST; }
void track1() {}

void track2(int level) {
  if (!fu2(level)) return;
  exit(2);
}

extern void fu(void *);
static int flag;
void test3_1() {
  void *p;

  p = malloc(100);
  if (flag) {
    fu(p);
  } else {
    track(HIGHEST);
  }
  // Interval 未跟踪 fun2 中的 level >= 6 导致 context 不正确，进而影响 checker
}  // required // option

void test3_2() {
  void *p;

  p = malloc(100);
  if (flag) {
    fu(p);
  } else {
    track(HIGH);
  }
}  // required  // required // option
