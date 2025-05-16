#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&t0,&id_dest->val,&id_src->val);

  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  //printf("[test] ZF=%d,SF=%d\n",cpu.ZF,cpu.SF);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);

  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);

  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);

  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_sar(&t0,&t0,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  t0 = id_dest->val;
  rtl_not(&t0);
  operand_write(id_dest,&t0);

  print_asm_template1(not);
}

make_EHelper(ror) //右循环移位
{
  uint32_t shamt = id_src->val & 0x1f; //只取低五位，避免超限
  t0 = id_dest->val;
  t0 = (t0>>shamt) | (t0<<((id_dest->width*8)-shamt));
  operand_write(id_dest,&t0);

  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(ror);
}

make_EHelper(rol) //左循环移位
{
  uint32_t shamt = id_src->val & 0x1f;
  t0 = id_dest->val;
  t0 = (t0<<shamt) | (t0>>((id_dest->width*8)-shamt));
  operand_write(id_dest,&t0);

  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(rol);
}

make_EHelper(shld)
{
  //左移，结果存到t0
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  rtl_li(&t2,id_src2->width);//t2
  rtl_shli(&t2,&t2,3);//t2 = t2 *8
  rtl_subi(&t2,&t2,id_src->val);//t2 = t2 - count
  rtl_shr(&t2,&id_src2->val,&t2);//获取补位
  rtl_or(&t0,&t0,&t2);//拼接
  operand_write(id_dest,&t0);//写回
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template3(shld);
}

make_EHelper(shrd)
{
  //右移
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  //t2 = id_src2->width
  rtl_li(&t2,id_src2->width);
  //t2=t2*8
  rtl_shli(&t2,&t2,3);
  //t2 =t2 - count
  rtl_subi(&t2,&t2,id_src->val);
  //t2 = src << (bit_width -count)
  rtl_shl(&t2,&id_src2->val,&t2);
  //拼接
  rtl_or(&t0,&t0,&t2);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template3(shrd);
}