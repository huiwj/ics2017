#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) { //虚拟到物理
  PDE* pg = &((PDE*)(p->ptr))[PDX(va)]; //页目录
  PTE* ptep;
  if(*pg & 0x1) //页表是否存在
  {
    ptep = (PTE*)PTE_ADDR(*pg);
  }
  else
  {
    ptep = (PTE*)palloc_f(); //分配新页表
    *pg = PTE_ADDR(ptep) | 0x1; //设置present
  }

  ptep[PTX(va)]=PTE_ADDR(pa) | 0x1; //设置页表项
  
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  uint32_t* ptr = ustack.end - 4; //用户栈的末尾

  //start函数栈帧为0
  for(int i =0;i<3;i++)
  {
    *ptr-- =0;
  }

  //设置陷阱帧
  *ptr-- = 0x202;//eflags：允许中断
  *ptr-- = 8;//cs
  *ptr-- = (uint32_t)entry;//eip
  *ptr-- = 0;//errorcode
  *ptr-- = 0x81;//irq

  //消除通用寄存器，初始化0
  for(int i=0;i<8;i++)
  {
    *ptr-- =0;
  }

  //指向陷阱帧开始
  ptr++;

  //设置栈顶指针 tf
  *(uint32_t*)(ustack.start) = (uint32_t)ptr;

  //返回陷阱帧
  return (_RegSet*)((uint32_t)ptr);
}
