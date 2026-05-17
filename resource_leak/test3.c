
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
}  

void test3_2() {
  void *p;

  p = malloc(100);
  if (flag) {
    fu(p);
  } else {
    track(HIGH);
  }
}    
