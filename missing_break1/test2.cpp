extern "C" {
void abort();
void exit(int _Code);
}

void llvm_unreachable(char* s) { abort(); }

static void AArch64MnemonicSpellCheck(char* x, int FBS, unsigned VariantID) {
  VariantID = 3;

  switch (VariantID) {
    default:
      llvm_unreachable("");
    case 0:
      FBS++;
      VariantID--;
      break;
    case 3:
      FBS++;
      VariantID--;
      break;
  }
}

static void test1(char* x, int FBS, unsigned VariantID) {
  VariantID = 3;

  switch (VariantID) {
    default:
      abort();

    case 0:
      FBS++;
      VariantID--;
      break;
    case 3:
      FBS++;
      VariantID--;
      break;
  }
}

static void test2(char* x, int FBS, unsigned VariantID) {
  VariantID = 3;

  switch (VariantID) {
    default:
      exit(1);

    case 0:
      FBS++;
      VariantID--;
      break;
    case 3:
      FBS++;
      VariantID--;
      break;
  }
}

void myExit() { exit(1); }

static void test3(char* x, int FBS, unsigned VariantID) {
  VariantID = 3;

  switch (VariantID) {
    default:
      myExit();

    case 0:
      FBS++;
      VariantID--;
      break;
    case 3:
      FBS++;
      VariantID--;
      break;
  }
}

void test4(int x) {
  switch (x) {
    case 1:
      break;
    case 2:
      x = 1;
      break;
    case 3:
      x += 1;
    case 4:
    case 5:
    case 6:
      break;
  }
}

void test5(int x) {
  switch (x) {
    case 1: {
      x++;
    }
    default:
        // x--;
        ;
  }
}

void test6(int x) {
  switch (x) {
    case 1: {
      x++;
    }
    default:
      x--;
      ;
  }
}

void test8(int x) {
  switch (x) {
    case 1: {
      x++;
    }
    default:;
      x--;
  }
}

void test10(int x) {
  switch (x) {
    case 1: {
      x++;
    }
    default:;
  }
}

void test11(int x) {
  switch (x) {
    case 1: {
      x++;
    }
    case 2:;
  }
}

void test3a(int x) {
  int y;
  switch (x) {
    case 1: {
      switch (x) {
        case 1:
          y--;
          goto lab;
        case 2:
          y++;
          goto lab;
        case 3:
          if (y > 0) {
            y++;
            goto lab;
          }
          if (y < 0) {
            y++;
            goto lab;
          }
          [[fallthrough]];
        default:
          y++;
          goto lab;
      }
    }
    case 2: {
      switch (x) {
        case 1:
          y--;
          goto lab;
        case 2:
          y++;
          return;
      }
    }
  }
lab:
  y++;
}

int test2a(int x, int y) {
  switch (x) {
    case 1: {
      switch (x) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          if (y > 0) return 0;
          [[fallthrough]];
        case 9:
        case 10:
          return 0444;
        default:
          return 0;
      }
    }
    case 2:
      y++;
      break;
  }
}

void test3(int x) {
  int y;
  switch (x) {
    case 1:
      switch (x) {
        case 1:
          y++;
          return;

        case 2: {
          y++;

          switch (x) {
            case 1:
              y++;
              break;
            case 2:
              y--;
              break;
            case 3:
              y--;
              break;
            default:
              return;
          }
          y--;
          return;
        }

        default:
          return;
      }
    case 2:
      y--;
      return;
  }
}

void test4x(int x) {
  int y;
  switch (x) {
    case 1:
      switch (x) {
        case 1:
          y++;
          return;

        case 2: {
          unsigned int scale;

          switch (x) {
            case 1:
              y--;
              break;
            case 2:
              y++;
              break;
            case 3:
              y--;
              break;
            default:
              return;
          }
          y--;

          return;

          default:
            return;
        }
      }
    case 2:
      y--;
      return;
  }
}

void test1r(int x) {
  switch (x) {
    default:
      for (;;) {
        break;
      }
    case 2:
      x--;
      break;
  }
}

void test2r(int x) {
  switch (x) {
    default:
      for (; x < 10;) {
      }
    case 2:
      x--;
      break;
  }
}

void test5r(int x) {
  switch (x) {
    default:
      for (;;) {
        if (x > 10) {
          break;
        }
      }
    case 2:
      x--;
      break;
  }
}

void test6r(int x) {
  switch (x) {
    default:
      while (x < 10) {
        return;
      }
    case 2:
      x--;
      break;
  }
}

void test7r(int x) {
  switch (x) {
    default:
      for (; x < 10;) {
        return;
      }
    case 2:
      x--;
      break;
  }
}

void test13r(int x) {
  switch (x) {
    default:
      do {
        break;
      } while (x < 10);
    case 2:
      x--;
      break;
  }
}

enum options { A, B, C, D, E };
bool test(options op) {
  int x;
  switch (op) {
    case A:
      x++;
      return true;
    case B: {
      x++;
      return true;
    }
    case C:
      if (x++) {
        return false;
      }
      // log_warn(x++;)
    case D: {
      if (x++) {
        x++;
      } else {
        x++;
      }
      x++;
      return true;
    }
    case E:
    default: {
      if (x++) {
        // do sth
      } else {
        x++;
      }
      x++;
      return true;
    }
  }
}

#define ut_ad(EXPR)

int dict_index_get_n_unique(int* index);
int error;
int* intable_apply_convert_mrec(int* log);

int test(unsigned short mode) {
  int* log;
  int* row;

  // int*	index	= log;

  ut_ad((log ? 0 : 2));

  intable_apply_convert_mrec(log);

  switch (error) {
    case 2:
      /* The record contained BLOBs that are now missing. */
      ut_ad(log->blobs);

    case 3:
      ut_ad(row != NULL);
      break;
    default:
      ut_ad(0);
    case 4:
      ut_ad(row == NULL);
      return error;
  }
  return *log;
}

int test1(unsigned short mode) {
  switch (error) {
    case 2:
      /* The record contained BLOBs that are now missing. */
      ut_ad(1);

    case 3:
      ut_ad(2);
      break;
    default:
      ut_ad(0);
    case 4:
      ut_ad(3);
      return error;
  }
  return 3;
}

void test(int k) {
  switch (k) {
    case 1:;
    case 2:;
      break;
  }
}
