/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test.c
 * @brief STRING_BUFFER_OVERFLOW 测试
 * @author Wang Caiyun ()
 */

char* StrCat(char* strDestination, const char* strSource) {
  strSource++;
  strDestination--;
}

void test4a(char* a) {
  StrCat("::", a);
}
