void read_buffer(char* buf, int offset) {
    char* ptr = buf + offset;  
    char c = *ptr;  
}

bool copy_name(char *dst, size_t dst_size, const char *src)
{
  strcpy(dst, src);
  return true;
}

char *get_char(char *str, int pos)
{
  return str[pos];
}

void test1(int index)
{
  int arr[10];
  int value = arr[index];
}

void test(unsigned len)
{
  char *buf = malloc(len);
  char *p = buf + len;
  while (p > buf && *(p - 1) == ' ')
    --p;
  char i = *(p - 1);
}

struct packet
{
  int length;
  char data[1];
};

void process_packet(struct packet *pkt)
{
  for (int i = 0; i < pkt->length; i++)
  {
    process_byte(pkt->data[i]);
  }
}
