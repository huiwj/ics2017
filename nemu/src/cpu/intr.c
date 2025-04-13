#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //计算中断描述符在IDT中地址
  uint32_t idt_base = cpu.idtr.base;
  uint32_t idt_limit = cpu.idtr.limit;
  uint32_t idt_entry_addr = idt_base + NO *8;

  assert(idt_entry_addr+7<=idt_base + idt_limit);
  //读取IDT表项
  uint32_t low = vaddr_read(idt_entry_addr,4);
  uint32_t high = vaddr_read(idt_entry_addr+4,4);
  //提取offset
  uint32_t offset_low = low & 0x0000FFFF;
  uint32_t offset_high = high &0xFFFF0000;
  uint32_t handler_addr = offset_high |offset_low;
  //压栈
  cpu.esp -=4;
  vaddr_write(cpu.esp,4,cpu.eflags);
  cpu.esp-=4;
  vaddr_write(cpu.esp,4,cpu.CS);
  cpu.esp-=4;
  vaddr_write(cpu.esp,4,ret_addr);

  printf("raise_intr:NO=0x%x,handler=0x%x\n",NO,handler_addr);
  
  //设置跳转指令
  decoding.jmp_eip = handler_addr;
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
}
