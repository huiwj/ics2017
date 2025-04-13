#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //计算中断描述符在IDT中地址
  uint32_t idt_base = cpu.idtr.base;
  uint32_t idt_entry_addr = idt_base + NO *8;

  //读取IDT表项
  uint32_t low = vaddr_read(idt_entry_addr,4);
  uint32_t high = vaddr_read(idt_entry_addr+4,4);

  //提取offset
  uint32_t offset_low = low & 0xFFFF;
  uint32_t offset_high = (high >> 16)&0xFFFF;
  uint32_t handler_addr = (offset_high << 16)|offset_low;

  //压栈
  cpu.esp -=4;
  vaddr_write(cpu.esp,4,cpu.eflags);

  cpu.esp-=4;
  vaddr_write(cpu.esp,4,ret_addr);

  cpu.esp-=4;
  vaddr_write(cpu.esp,4,cpu.CS);

  //设置跳转指令
  cpu.eip = handler_addr;
  cpu.CS = 8;
}

void dev_raise_intr() {
}
