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

WP* new_wp(){

  if(free_==NULL){

    printf("NO watchpoint left!\n");

    return NULL;

  }

  if(head==NULL){

    head=free_;

    free_=free_->next;

    head->next=NULL;

    return head;

  }

  else{

    WP* cur=head;

    while(cur->next!=NULL){

        cur=cur->next;

    }

    cur->next=free_;

    free_=free_->next;

    cur->next->next=NULL;

    return cur->next;

  }

}



void free_wp(WP *wp){

  WP *cur=head;
  
  if(head==wp){
printf("find target\n");
  head=head->next;
  wp->next=free_;

  free_=wp;
}

  while(cur!=NULL){

    if(cur->next==wp){

      printf("find target\n");

      cur->next=cur->next->next;

      wp->next=free_;

      free_=wp;

      break;

    }

    cur=cur->next;

  }

  wp->val=0;

  wp->expr[0]='\0';

}



void print_w() {

	WP *cur = head;

	while (cur != NULL) {

		printf("[Watchpoint NO.%d]\tExpression: %s\tValue: %d\n", cur -> NO, cur -> expr, cur -> val);

		cur = cur -> next;

	}

}



WP* find_wp(int n){

  WP *cur = head;

	while (cur != NULL) {

    if(cur->NO==n){

      return cur;

    }

    cur = cur -> next;

	}

  return NULL;

}



bool check_wp() {
	bool flag=false;

	WP* cur = head;

	while (cur != NULL) {

		bool tmp = true;

		uint32_t new_val = expr(cur->expr, &tmp);

		if (new_val != cur -> val) {

			printf("[Watchpoint NO.%d]\tExpression: %s\tOrigin Value: 0x%x\tNew Value: 0x%x\n", cur -> NO, cur -> expr, cur -> val, new_val);

			cur -> val = new_val;

			flag=true;

		}

		cur = cur -> next;

	}
	return flag;
}
