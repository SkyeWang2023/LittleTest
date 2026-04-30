#include <stdlib.h>
void test(size_t size){
  size = 100;
  char* p = (char*) malloc(size);
  // 可以执行其他操作
  free(p);
}

void getSize(size_t* size){
   size = 100;
}

void test1(size_t size){
  getSize(&size);
  char* p = (char*) malloc(size);
  // 可以执行其他操作
  free(p);
}

void test2(size_t size){
  if(size > 200){
    return;
  }
  char* p = (char*) malloc(size);
  // 可以执行其他操作
  free(p);
}