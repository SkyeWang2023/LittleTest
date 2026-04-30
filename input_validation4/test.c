#include <stdio.h>
#include <stdlib.h>

int mysystem(char* cmd)
{
    system(cmd);
    return 0;
}

void test(char* cmd){
  mysystem(cmd); //#defect#input_validation
  return 0;
}