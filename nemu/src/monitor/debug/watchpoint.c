#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void print_watchpoints()
{
  printf("监视点信息：\n");
  for(WP *temp = head;temp !=NULL;temp = temp->next)
  {
    printf("%d\t%s\t\t0x%08x\n",temp->NO,temp->expr,temp->value);
  }
}

//申请空闲监视点
WP* mew_wp()
{
  assert(free_ !=NULL);

  WP *temp = free_;
  free_ = free_->next;

  temp -> next = head;
  head = temp;

  return temp;
}


//释放监视点
void free_wp(WP *wp)
{
  if(head == wp)
  {
    head = wp->next;
  }
  else
  {
    WP *prev = head;
    while(prev != NULL && prev->next != wp)
    {
      prev = prev ->next;
    }

    assert(prev !=NULL); //确保wp在链表中
    prev->next = wp->next;
  }

  //返回到空闲链表中
  wp->next = free_;
  free_ = wp;
  
}

void add_point(char *args)
{
  bool success = false;
  WP *wp = new_wp();
  strncpy(wp->expr,args,127);
  wp->expr[127]=0;

  wp -> value = expr(args,&success);
  assert(success);
  printf("监视点创建成功");
}

void check_point()
{
  WP *wp = head;
  bool success = false ;

  while (wp!=NULL)
  {
    uint32_t new_value = expr(wp->expr,&success);
    assert(success);

    if(new_value != wp->value)
    {
      printf("触发监视点 #%d:%s\n",wp->NO,wp->expr);
      printf("旧值 = %u, 新值 = %u\n",wp->value,new_value);
      wp->value = new_value;
      
    }
    wp=wp->next;
  }
  
}


void remove_point(int numb)
{
  WP *wp = head;
  while(wp!=NULL)
  {
    if(wp->NO == numb)
    {
      free_wp(wp);
      printf("删除监视点\n");
      return;
    }

    wp = wp->next;
  }

  printf("未找到监视点\n");
}
