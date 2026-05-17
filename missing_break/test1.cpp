/**
 * Copyright (C) 2023 Software Security Co., Ltd. - All Rights Reserved
 *
 * @file test1.cpp
 * @brief MISSING_SWITCH_BREAK 测试
 * @author Wang caiyun
 */
extern "C" {
void abort();
void exit(int _Code);
int printf(const char* fmt, ...);
}

#define TESTCASE(x, y) \
  case ((x)):          \
    (y) = (x) + 1;

#define TESTBREAK break;

void test1(int x) {
  switch (x) {
    case 1:
      break;
    case 2:
      return;
  }
}

void test2(int x) {
  switch (x) {
    case 1:
      break;
    case 2:
      break;
      x++;
    case 3:
      return;
  }
}

void test3(int x) {
  int y;
  switch (x) {
    TESTCASE(1, y)
    TESTBREAK
    TESTCASE(2, y)
    TESTCASE(3, y)
  }
}

void test4(int x) {
  switch (x) {
    case 1:
      x += 1;
      break;
    case 2:
      x--;
      break;
    case 3:
      x++;
  }
}

void test5(int x) {
  switch (x) {
    case 1:
      x += 1;
    case 2:
      x--;
      break;
    case 3:
      x++;
      break;
  }
}

void test6(int x) {
  switch (x) {
    case 1:
      x += 1;
      switch (x) {
        case 4:
          x--;
        case 5:
          x = x + 10;
          x--;
          return;
      }
      break;
    case 2:
      x--;
      break;
    case 3:
      x++;
      break;
  }
}

void test7(int x) {
  switch (x) {
    case 1:
    {
      x += 1;
      break;
      x--;
    }
    case 2:
      x--;
      break;
    case 3:
      x++;
      break;
  }
}

void test8(int x) {
  switch (x) {
    case 1: {
      x += 1;
      x--;
    }
    case 2:
      x--;
      break;
    case 3:
      x++;
      break;
  }
}

void test9(int x) {
  switch (x) {
    case 1:
      x--;
      break;
    case 2:
      x += 1;
      switch (x) {
        case 4:
          x--;
          break;
        case 5:
          return;
      }

    case 3:
      x++;
      break;
  }
}

void test10(int x) {
  switch (x) {
    case 1:
      x--;
      break;
    case 2:
      x += 1;
      switch (x) {
        case 4:
          x--;
          break;
        case 5:
          return;
      }
    default:
      x++;
  }
}

void test11(int x) {
  int k = 9;
  switch (x) {
    case 1:
      k++;
    case 2:
      k++;
    case 3:
      k++;
    case 4:
      k--;
      return;
  }
}

void test12(int x) {
  int k = 9;
  int b = 20;
  switch (x) {
    case 1:

    case 2:

    case 3:
      k++;
    case 4:
      k--;
      return;
  }
}

void test13(int x) {
  int k = 9;
  int b = 20;
  switch (x) {
    case 1:
    {
      b++;
      {
        k++;
        break;
      }
    }

    case 2:
      k++;
    case 3:
      k++;
    case 4:
      k--;
      return;
  }
}

void test16(int x) {
  int k = 9;
  int b = 20;
  switch (x) {
    case 1:
      k = b;
  }
}

void test17(int x) {
  int k = 9;
  switch (x) {
    case 1:
    {
    }
    case 2: {
    }
    default:
      k++;
  }
}

void test18(int x) {
  int k = 9;
  switch (x) {
    case 1:
    // {
    // }
    case 2: {
      k--;
      break;
    }
    default:
      k++;
  }
}
void test19(int x) {
  int k = 9;
  switch (x) {
    case 1:
    {
    }
      {} {}
    default:
      k++;
  }
}

void test20(int x) {
  int k = 9;
  switch (x) {
    case 1:
    {
      k--;
      {
        k++;
        { exit(0); }
      }
    }
    default:
      k++;
  }
}

void test21(int x) {
  int k = 9;
  switch (x) {
    case 1:
    {
      k--;
      {
        k++;
        { goto lab; }
      }
    }
    default:
      k++;
  }

lab:
  k++;
}

void test22(int x) {
  int k = 9;
  switch (x) {
    case 1:
    {
      k--;
      {
        k++;
        { abort(); }
      }
    }
    default:
      k++;
  }
}

void test29(int x) {
  int k = 9;
  for (;;) {
    switch (x) {
      case 1:
        continue;
      case 2:
        x++;
    }
  }
}

void test30(int x) {
  switch (x) {
    default:
      x++;
    case 1:
      x--;
  }
}

void test31(int x) {
  switch (x) {
    case 0:
      x++;
      break;
    default:
      x++;
    case 1:
      x--;
  }
}

void test32(int x) {
  int k = 9;
  switch (x) {
    case 3: {
      x--;
      {{} {}

      } {}
    }

    case 4: {
      {{} {x++;
      {}
    }
  }
  {}
}
case 5:
  x++;
  }
  }

  void test33(int x) {
    int k = 9;
    switch (x) {
      case 3: {
        {{} {}

        } {
        }
        x--;
      }

      case 4: {
        {{} {x++;
        {}
      }
    }
    {}
  }

default:
  k++;
  }
  }

  void test191(int x) {
    int k = 9;
    switch (x) {
      case 1:
      {
      }
        {} {}
      case 2:
      {
        {
          {}
        }
      }

      case 3: {
        {{} {}

        } {
        }
      }

      case 4: {
        {{} {x++;
        {}
      }
    }
    {}
  }

default:
  k++;
  }
  }

  enum class NUM { one, two };
  void test8k(NUM x, int* p) {
    switch (x) {
      case NUM::one:
        *p++;
      case NUM::two:
        *p++;
    }
  }
