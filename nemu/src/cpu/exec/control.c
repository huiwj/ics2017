#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  //TODO();
  rtl_li(&t2, decoding.seq_eip);

  rtl_push(&t2);

  decoding.is_jmp = 1;

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  //TODO();
  rtl_pop(&t0);
  decoding.jmp_eip=t0;
  decoding.is_jmp=1;
  print_asm("ret");
}

make_EHelper(call_rm) {
  //TODO();
  //计算返回地址
  rtl_li(&t0,decoding.seq_eip);
  rtl_push(&t0);//压入返回地址

  //跳转到目标地址
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1; 

  print_asm("call *%s", id_dest->str);
}
