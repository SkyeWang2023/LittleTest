/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test1.c
 * @brief RESOURCE_LEAK 测试
 * @author 史林霞 (shilinxia@softsafe-tech.com)
 */

extern void *malloc(unsigned long long size);
extern void free(void *p);

int test1_1(int delta) {
  unsigned count = 0;
  count += delta;
  char *p = malloc(4);
  if ((int)count < 0) {
  } else {
    free(p);
  }
  return count;
}
