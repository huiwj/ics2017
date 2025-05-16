#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int count = 0;
bool cur_gam = 1;


_RegSet* schedule(_RegSet *prev) {
  /* 
  current->tf = prev;

  current = (count == 10 ? &pcb[1] :(cur_gam ? &pcb[0]:&pcb[2]));
  int pcb_id = (current == &pcb[0] ? 0:(current == &pcb[1] ? 1:2));
  Log("pcb[%d]",pcb_id);
  if(count == 10)
  {
    count = 0;
  }

  count++;
  Log("PTR:0x%x\n",(uint32_t)current->as.ptr);

  _switch(&current->as);

  return current->tf;
  */
 return NULL;

  
}

