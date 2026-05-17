/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test2.cc
 * @brief RESOURCE_LEAK 测试
 * @author 史林霞 (shilinxia@softsafe-tech.com)
 */
#include <vector>

using namespace std;

void *get() {
  void *p = new char[24];
  vector<void *> v;
  v.push_back(p);
  return p;
}

void test2_2() {
  void *p;
  p = get();
}  
