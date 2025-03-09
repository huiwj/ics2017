#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[128]; //表达式
  uint32_t value; //上次计算值

  /* TODO: Add more members if necessary */


} WP;

void print_watchpoints();


#endif
