void test01(int s, void* p) {
  recv(s, p, 30, 1);
  if (p) {
    strlen((char*)p);  
  }
}
