/**
 * Copyright (C) 2024 Software Security Co., Ltd. - All Rights Reserved.
 *
 * @file test08.cpp
 * @brief
 * @author 史林霞 (shilinxia@softsafe-tech.com)
 */

extern "C" {
extern void free(void *p);
extern int *realloc(int size);
extern void *malloc(int size);
extern void exit(int status);
}

#define NULL ((void *)0)
void test8(void *p) {
  do {
    p = malloc(2);
    free(p);
  } while (p != NULL);  // not required
}

void test8_1(void *p) {
  p = malloc(2);
  free(p);
  if (p != NULL)  // not required
  {
    int a = 3;
  }
}

void test8_2(void *p, int a) {
  p = malloc(2);
  free(p);
  if (p != &a)  // not required
  {
    int a = 3;
  }
}

void test8_21(void *p, int a) {
  p = malloc(2);
  free(p);
  if (p > &a)  // required
  {
    int a = 3;
  }
}

void test8_22(void *p, int a) {
  p = malloc(2);
  free(p);
  if (p == &a)  // not required
  {
    int a = 3;
  }
}

void test8_3() {
  int *p = (int *)malloc(3 * sizeof(int));
  int *p2 = p;

  free(p);

  int count = p2 - p;  // not required
  p;                   // required
}

void test8_4(int *p, int *q) {
  free(q);
  p - q;  // not required
}

void test8_41(int *p, char *q) {
  free(q);
  p - (int *)q;  // required
}

void test8_5(int *p, int *q) {
  free(p);
  p - q;  // not required
}

void test8_6(int *p, int *q) {
  free(p);
  auto count = p - q;  // not required
}

void test8_7(int *p, int *q) {
  free(p);
  p + 3;  // required
}

void test8_8(int *p, int *q) {
  free(p);
  p++;  // required
}

#define NULL ((void *)0)
typedef unsigned long long size_t;

extern void *malloc(size_t size);
extern void free(void *ptr);

// ===========Case 1 =========
// 取链表head
extern void *channel_peek();
// 取出链表head， head设为下一个元素
extern void *channel_get();

// 函数channel_get每次返回的都不是同一个指针
void test08_31() {
  while (channel_peek() != NULL) {
    free(channel_get());  // not required
  }
}

// ========= Case 2 =========
struct node {
  int i;
  struct node *next;
};

void free_next(struct node *head, struct node *node) {
  head->next = node->next;
  free(node);
}

// 表达式head->next循环每次也是不同的值
void test08_32(struct node *head) {
  while (head->next != NULL) {  // not required
    free_next(head, head->next);
  }
}

// ========== Case 3 ========
struct node *g_head;
void free_node(struct node *node) {
  g_head = node->next;
  free(node);
}
// g_head 为全局变量，free_node后已经更新了
void test08_33() {
  while (g_head != NULL) {  // not required
    free_node(g_head);
  }
}

// ========== Case 4 ========
void free_node_02(struct node *node) {
  static struct node *g_head = node->next;
  free(node);
}
// g_head 为全局变量，free_node后已经更新了
void test08_34() {
  while (g_head != NULL) {  // #fndefect#USE_AFTER_RELEASE // SAST-2957
    free_node_02(g_head);
  }
}

// ========== Case 5 ========
void free_node_03(struct node *node) {
  struct node *g_head = node->next;
  free(node);
}
// g_head 为全局变量，free_node后已经更新了
void test08_35() {
  while (g_head != NULL) {  // #fndefect#USE_AFTER_RELEASE // SAST-2957
    free_node_03(g_head);
  }
}

int cond1();
int cond2();

void test08_36(int x, void *p) {
  p = malloc(2);

  if (cond1()) {
    free(p);
    exit(-1);
  }

  if (cond2()) {
    free(p);  // not required
    exit(-1);
  }
}
