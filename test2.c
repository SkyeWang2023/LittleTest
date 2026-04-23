void test(unsigned len, char *buf)
{
  char *p = buf + len;
  while (p > buf && *(p - 1) == ' ')
    --p;
  char i = *(p - 1);
}
