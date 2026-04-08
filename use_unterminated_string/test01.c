/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test01.c
 * @brief USE_UNTERMINATED_STRING 测试
 * @author Wang caiyun
 */

#include "header.h"

void test01(int s, void* p) {
  recv(s, p, 30, 1);
  if (p) {
    strlen((char*)p);  // #defect#USE_UNTERMINATED_STRING
  }
}

void test02(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[19] == '\0') {
    strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test03(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[0] == '\0') {
    strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test04(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[0] == 0) {
    strlen(buf);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test05(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[0] == 1) {
    strlen(buf);  // #defect#USE_UNTERMINATED_STRING
  }
}

void test06(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[0] != 0) {
    strlen(buf);  // #defect#USE_UNTERMINATED_STRING
  }
}

void test07(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[0] != 0) {
    strlen(buf);  // #defect#USE_UNTERMINATED_STRING
  } else {
    // different, nuwa is correct
    strlen(buf);  // #fndefect#USE_UNTERMINATED_STRING //SAST-3228
  }
}

void test08(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[4] != '\0') {
    return;
  }
  // different, nuwa is correct
  strlen(buf);  // #fndefect#USE_UNTERMINATED_STRING //SAST-3228
}

void test08a(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (buf[4] != '\0') {
    return;
  }
  // different, nuwa is correct
  strlen(buf);  // #fndefect#USE_UNTERMINATED_STRING //SAST-3228
 
}


void test09(int s) {
  char buf[20];

  recv(s, buf, 30, 1);
  if (0 == buf[3]) {
    strlen(buf); // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test10(int s, void* p) {
  recv(s, p, 30, 1);
  if (((char*)p)[5] == 0) {
    strlen((char*)p);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test11(int s, void* p) {
  recv(s, p, 30, 1);

  strlen((char*)(p)+3);  // #defect#USE_UNTERMINATED_STRING
  
}


void test12(int s, void* p) {
  recv(s, p, 30, 1);

  strlen((char*)(p)-3);  // #defect#USE_UNTERMINATED_STRING
  
}

void test13(int s, void* p) {
  recv(s, p, 30, 1);

  if(*((char*)p)){
    strlen((char*)(p)-3);  // #defect#USE_UNTERMINATED_STRING
  }
}


void test14(int s, void* p) {
  recv(s, p, 30, 1);

  if(*((char*)p)){
    strlen((char*)(p));  // #defect#USE_UNTERMINATED_STRING
  }
}


void test15(int s, char* p) {
  recv(s, p, 30, 1);
  
  *p == 0;

  strlen((char*)(p));  // #defect#USE_UNTERMINATED_STRING
 
}


void test16(int s, char* p) {
  recv(s, p, 30, 1);

  if(*p){
    strlen(p-3);  // #defect#USE_UNTERMINATED_STRING
  }
}


void test17(int s, char* p) {
  recv(s, p, 30, 1);

  if(*p == 0){
    strlen(p-3);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test18(int s, void* p) {
  recv(s, p, 30, 1);

  if(!*((char*)p)){
    // different, nuwa is correct
    strlen((char*)(p));  // #fndefect#USE_UNTERMINATED_STRING //SAST-3228
  }
}

void test19(int s, char* p) {
  recv(s, p, 30, 1);
  
  *p = 0;

  strlen((char*)(p));  // #nodefect#USE_UNTERMINATED_STRING
 
}


void test20(int s, char* p) {
  recv(s, p, 30, 1);
  
  p++;

  strlen((char*)(p));  // #defect#USE_UNTERMINATED_STRING
 
}

void test21(int s, char* p) {
  recv(s, p, 30, 1);
  
  memset(p,0,100);

  strlen((char*)(p));  // #nodefect#USE_UNTERMINATED_STRING
 
}

void *myMemset(void *dest, int ch, unsigned long long count){
  memset(dest,ch,count);
}

void test21a(int s, char* p) {
  recv(s, p, 30, 1);
  
  myMemset(p,0,100);

  strlen((char*)(p));  // #nodefect#USE_UNTERMINATED_STRING
 
}


typedef struct A{
  char ary[100];
}SA;

void test22(int s, char* p) {
  SA a;
  recv(s, a.ary, 30, 1);
  
  memset((char*)&a,0,100);

  // Need not fix,same
  strlen(a.ary);  // #fpdefect#USE_UNTERMINATED_STRING //SAST-2812
 
}

void gc4(void) {
 char src[] = "welcome home";
 char dest[13];
 char* p = dest;
 strncpy(p, src, 6);
 strlen(dest); // #nodefect#USE_UNTERMINATED_STRING
}


void gc4a(void) {
  char src[] = "welcome home";
  char dest[13];
  char* p = dest;
  strncpy(p, src, 6);
  strlen(p); // #defect#USE_UNTERMINATED_STRING
}

void gc4b(void) {
 char src[] = "welcome home";
 char dest[13];
 char* p = src;
 strncpy(dest, p, 6);
 strlen(dest); // #nodefect#USE_UNTERMINATED_STRING
}

void gc4c(void) {
  char src[] = "welcome home";
  char dest[13];
  char* p = dest;
  src[2]=0;
  strncpy(p, src, 6);
  strlen(p); // #defect#USE_UNTERMINATED_STRING
}


#define DHD_PRIV_EVENT_MSG_44_1 "wl event_msg 44 1"
#define SOCK_DGRAM 2

int ioctl(int d, unsigned long request, ...);

typedef unsigned int socklen_t;

typedef unsigned int __u32;

typedef unsigned short __u16;

typedef __u16 __be16;

typedef __u32 __be32;

#define __ss_aligntype	unsigned long int

struct in_addr {
  __be32 s_addr;
};

typedef unsigned short __kernel_sa_family_t;

#define AF_INET 2

struct sockaddr_in {
  __kernel_sa_family_t sin_family; /* Address family		*/
  __be16 sin_port;                 /* Port number			*/
  struct in_addr sin_addr;         /* Internet address		*/
};

typedef __kernel_sa_family_t sa_family_t;

struct sockaddr {
  sa_family_t sa_family; /* address family, AF_xxx	*/
  char sa_data[14];      /* 14 bytes of protocol address	*/
};


extern int socket(int __domain, int __type, int __protocol);
extern unsigned long long strlen(const char *str);
typedef struct dhd_priv_cmd
{
    char *buf;
    int used_len;
    int total_len;
} dhd_priv_cmd;

typedef void *__caddr_t;

struct ifreq {
#define IFHWADDRLEN 6
#define IFNAMSIZ 16
union {
char ifrn_name[IFNAMSIZ]; /* 接口名称，例如 "eth0" */
} ifr_ifrn;

union {
struct sockaddr ifru_addr;
struct sockaddr ifru_dstaddr;
struct sockaddr ifru_broadaddr;
struct sockaddr ifru_netmask;
struct sockaddr ifru_hwaddr;
short int ifru_flags;
int ifru_ivalue;
int ifru_mtu;
// struct ifmap ifru_map;
char ifru_slave[IFNAMSIZ];
char ifru_newname[IFNAMSIZ];
__caddr_t ifru_data;
} ifr_ifru;
};

#define ifr_name ifr_ifrn.ifrn_name /* 接口名称 */
#define ifr_hwaddr ifr_ifru.ifru_hwaddr /* MAC 地址 */
#define ifr_addr ifr_ifru.ifru_addr /* 地址 */
#define ifr_dstaddr ifr_ifru.ifru_dstaddr /* 点对点地址 */
#define ifr_broadaddr ifr_ifru.ifru_broadaddr /* 广播地址 */
#define ifr_netmask ifr_ifru.ifru_netmask /* 网络掩码 */
#define ifr_flags ifr_ifru.ifru_flags /* 标志 */
#define ifr_metric ifr_ifru.ifru_ivalue /* 测度 */
#define ifr_mtu ifr_ifru.ifru_mtu /* MTU */
#define ifr_map ifr_ifru.ifru_map /* 设备映射 */
#define ifr_slave ifr_ifru.ifru_slave /* 从设备 */
#define ifr_data ifr_ifru.ifru_data /* 接口使用 */
#define ifr_ifindex ifr_ifru.ifru_ivalue /* 接口索引 */
#define ifr_bandwidth ifr_ifru.ifru_ivalue /* 链路带宽 */
#define ifr_qlen ifr_ifru.ifru_ivalue /* 队列长度 */
#define ifr_newname ifr_ifru.ifru_newname /* 新名称 */

// dhd_priv_ioctl
int dhd_priv_ioctl(const char *ifname, void *buf, unsigned int len)
{
    struct ifreq ifr;
    dhd_priv_cmd priv_cmd;
    int ret = -1;
    int s = -1;
    char bufdata[20];

    memset((void *)&ifr, 0, sizeof(ifr));
    memset((void *)&priv_cmd, 0, sizeof(priv_cmd));
    strncpy(ifr.ifr_name, ifname, (sizeof(ifr.ifr_name) - 1));

    memset((void *)&bufdata, 0, sizeof(bufdata));
    memcpy((void *)&bufdata, (void *)buf, len);

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        return -1;
    }

    priv_cmd.buf = (char *)bufdata;

    ifr.ifr_data = (char *)&priv_cmd;

    if ((ret = ioctl(s, 0x89F0 + 1, &ifr)) < 0)
    {
      // strlen(bufdata);
        printf("IOCTL SIOCDEVPRIVATE+1:, if[%s] fd[%d] buflen[%lu]\n", ifr.ifr_name, s, strlen(bufdata));
    }

    // close(s);
    return ret;
}
void test(const char *ifname){
  if(dhd_priv_ioctl(ifname,DHD_PRIV_EVENT_MSG_44_1,strlen(DHD_PRIV_EVENT_MSG_44_1))<0){} // #nodefect#USE_UNTERMINATED_STRING
}


#define DEVICE_FILE_SIZE 100

static int RemoveDeviceNode(const char *deviceNode, char **symLinks)
{
    if (symLinks != NULL) {
        for (int i = 0; symLinks[i] != NULL; i++) {
            unsigned char realPath[DEVICE_FILE_SIZE] = {0};
            const char *linkName = symLinks[i];
            long long ret = readlink(linkName, realPath, 99);
            if (ret < 0) {
                continue;
            }
            strlen(realPath);  // #nodefect#USE_UNTERMINATED_STRING
        }
    }
}

int testc1(int len, char* ent_model) {
  char damageModel[64];
  char useModel[64];
  len = 7;
  strncpy(damageModel, ent_model, sizeof(damageModel));
  damageModel[len] = 0;  // damageModel[53] = '\0'

  // 拷贝到 useModel
  strncpy(useModel, damageModel, sizeof(useModel));
  // nuwa是正确的
  strlen(useModel);  // #nodefect#USE_UNTERMINATED_STRING //__DIFF__
}

int testc2(int len, char* ent_model) {
  char damageModel[64];
  char useModel[64];
  strncpy(damageModel, ent_model, sizeof(damageModel));
  damageModel[len] = 0;  // damageModel[53] = '\0'

  // 拷贝到 useModel
  strncpy(useModel, damageModel, sizeof(useModel));
  // nuwa是正确的
  strlen(useModel);  // #nodefect#USE_UNTERMINATED_STRING //__DIFF__
}

wchar_t *cur_msg = 0;
unsigned long long cur_msg_size = 1024;
unsigned long long cur_msg_len = 0;
void test_02(void) {
  wchar_t *temp;
  unsigned long long temp_size;
  if (cur_msg != 0) {
    temp_size = cur_msg_size / 2 + 1;
    temp = realloc(cur_msg, temp_size * sizeof(wchar_t));

    if (temp == 0) {
    }
    cur_msg = temp;
    cur_msg_size = temp_size;
    cur_msg_len = wcslen(cur_msg);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void test_02a(void) {
  wchar_t *temp;
  unsigned long long temp_size;
  temp_size = cur_msg_size / 2 + 1;
  temp = realloc(cur_msg, temp_size * sizeof(wchar_t));
  cur_msg_len = wcslen(temp);  // #nodefect#USE_UNTERMINATED_STRING
}

void test_02c(char *p) {
  char src[20] = {'a', 'b'};
  char *q = strncpy(p, src, 2);
  strlen(q);  // #defect#USE_UNTERMINATED_STRING
}

void testa_02(void) {
  wchar_t *temp;
  unsigned long long temp_size;
  if (cur_msg != NULL) {
    temp_size = cur_msg_size / 2 + 1;
    temp = realloc(cur_msg, temp_size * sizeof(wchar_t));
    if (temp == NULL) {
    }
    cur_msg = temp;
    cur_msg[temp_size - 1] = L'\0';
    cur_msg_size = temp_size;
    cur_msg_len = wcslen(cur_msg);  // #nodefect#USE_UNTERMINATED_STRING
  }
}

void bc3e(void) {
  unsigned long long size = 10;
  char *str = (char *)malloc(sizeof(char) * size);

  strlen(realloc(str, size * 2));  // #nodefect#USE_UNTERMINATED_STRING
}

void bc3f(char *str) {
  unsigned long long size = 10;

  strlen(realloc(str, size * 2));  // #nodefect#USE_UNTERMINATED_STRING
}

void *myRealloc(void *ptr, unsigned long long new_size) {
  return realloc(ptr, new_size);
}

void bc3g(char *str) {
  unsigned long long size = 10;

  strlen(myRealloc(str, size * 2));  // #nodefect#USE_UNTERMINATED_STRING
}

void bc3a(void) {
  unsigned long long size = 10;
  char *str = (char *)malloc(sizeof(char) * size);
  char *tmp = realloc(str, size / 2);
  strlen(tmp);  // #nodefect#USE_UNTERMINATED_STRING
}

void bc3b(void) {
  unsigned long long size = 10;
  char *str = (char *)malloc(sizeof(char) * size);

  strlen(realloc(str, size / 2));  // #nodefect#USE_UNTERMINATED_STRING
}
