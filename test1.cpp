void test2(int* p){
  p = nullptr;
  int x = 9;
  p = &x;
  *p = 2;
}
