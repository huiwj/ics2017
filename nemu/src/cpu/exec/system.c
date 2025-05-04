#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

extern void raise_intr(uint8_t NO,vaddr_t ret_addr);

make_EHelper(lidt) {
  //TODO();
  rtl_li(&t0,id_dest->addr);

  cpu.idtr.limit = vaddr_read(t0,2);

  cpu.idtr.base = vaddr_read(t0+2,4);
  if(decoding.is_operand_size_16)
  {
    cpu.idtr.base &= 0x00ffffff;
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();

  switch (id_dest->reg)
  {
  case 0:
    cpu.CR0 = id_src->val;
    break;
  case 3:
    cpu.CR3 = id_src->val;
  
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  switch (id_src->reg)
  {
  case 0:
    operand_write(id_dest,&cpu.CR0);
    break;
  case 3:
    operand_write(id_dest,&cpu.CR3);
    break;
  
  }

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val,decoding.seq_eip);//调用中断

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&decoding.jmp_eip);
  rtl_pop(&cpu.CS);
  rtl_pop(&cpu.eflags);
  decoding.is_jmp=1;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  //从src读取端口地址，并读取数值
  rtl_li(&t0,pio_read(id_src->val,id_dest->width));
  //数据写入目标操作数
  operand_write(id_dest,&t0);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
make_EHelper(out) {
  //TODO();
  //src读取端口地址，从dest读取需要写入的值
  pio_write(id_dest->val,id_dest->width,id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
