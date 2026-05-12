#include <stdlib.h>
void test(size_t size){
  char* p = (char*) malloc(size);
  // 可以执行其他操作
  free(p);
}
