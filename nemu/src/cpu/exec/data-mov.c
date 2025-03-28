#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_push(&t0);

  print_asm_template1(push);
  printf("push:esp: esp=0x%08x,val=0x%08x\n",cpu.esp,id_dest->val);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&t0);
  operand_write(id_dest,&t0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  rtlreg_t origin_esp = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&origin_esp);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  //跳过esp
  rtlreg_t dummy;
  rtl_pop(&dummy);

  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&dummy);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    //CWD:ax->dx:ax
    cpu.edx = (cpu.eax >> 15)? 0xFFFF : 0x0000;
  }
  else {
    //TODO();
    //CDQ:EAX -> EDX:EAX
    cpu.edx = (cpu.eax >> 31 ) ? 0xFFFFFFFF : 0x00000000;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
    //AL -> AX
    rtl_sext(&cpu.eax,&cpu.eax,1);
  }
  else {
    //TODO();
    //AX -> EAX
    rtl_sext(&cpu.eax,&cpu.eax,2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
/*
(nemu) si
  100000:   bd 00 00 00 00                        movl $0x0,%ebp
(nemu) si
  100005:   bc 00 7c 00 00                        movl $0x7c00,%esp
(nemu) si
[Call] before:EIP=0x0010000f,ESP=0x00007c00
)[Call] after:pushing ret = 0x0010000f,new ESP=0x00007bfc,target=0x0010001c
  10000a:   e8 0d 00 00 00                        call 10001c
(nemu) si
push:esp: esp=0x00007bf8,val=0x00000000
  10001c:   55                                    pushl %ebp
(nemu) si
  10001d:   89 e5                                 movl %esp,%ebp
(nemu) si
  10001f:   83 ec 08                              subl $0x8,%esp
(nemu) si
[Call] before:EIP=0x00100027,ESP=0x00007bf0
)[Call] after:pushing ret = 0x00100027,new ESP=0x00007bec,target=0x00100054
  100022:   e8 2d 00 00 00                        call 100054
(nemu) si
  100054:   8d 4c 24 04                           leal 0x4(%esp),%ecx
(nemu) si
  100058:   83 e4 f0                              andl $0xfffffff0,%esp
(nemu) si
push:esp: esp=0x00007bdc,val=0x00100027
  10005b:   ff 71 fc                              pushl -0x4(%ecx)
(nemu) si
push:esp: esp=0x00007bd8,val=0x00007bf8
  10005e:   55                                    pushl %ebp
(nemu) si
  10005f:   89 e5                                 movl %esp,%ebp
(nemu) si
push:esp: esp=0x00007bd4,val=0x586667b1
  100061:   57                                    pushl %edi
(nemu) si
push:esp: esp=0x00007bd0,val=0x46340154
  100062:   56                                    pushl %esi
(nemu) si
push:esp: esp=0x00007bcc,val=0x3726bb7b
  100063:   53                                    pushl %ebx
(nemu) si
push:esp: esp=0x00007bc8,val=0x00007bf0
  100064:   51                                    pushl %ecx
(nemu) si
  100065:   83 ec 08                              subl $0x8,%esp
(nemu) si
  100068:   31 ff                                 xorl %edi,%edi
(nemu) si
  10006a:   66 90                                 nop
(nemu) si
  10006c:   8d 34 3f                              leal (%edi,%edi,1),%esi
(nemu) si
  10006f:   31 db                                 xorl %ebx,%ebx
(nemu) si
  100071:   8d 76 00                              leal 0(%esi),%esi
(nemu) si
  100074:   83 ec 0c                              subl $0xc,%esp
(nemu) si
  100077:   8b 87 e0 01 10 00                     movl 0x1001e0(%edi),%eax
(nemu) si
  10007d:   03 83 e0 01 10 00                     addl 0x1001e0(%ebx),%eax
(nemu) si
  100083:   3b 84 b3 e0 00 10 00                  cmpl 0x1000e0(%ebx,%esi,4),%eax
(nemu) si
  10008a:   0f 94 c0                              sete %al
(nemu) si
  10008d:   0f b6 c0                              movzxl %al,%al
(nemu) si
push:esp: esp=0x00007bb0,val=0x00000001
  100090:   50                                    pushl %eax
(nemu) si
[Call] before:EIP=0x00100096,ESP=0x00007bb0
)[Call] after:pushing ret = 0x00100096,new ESP=0x00007bac,target=0x0010002c
  100091:   e8 96 ff ff ff                        call 10002c
(nemu) si
push:esp: esp=0x00007ba8,val=0x00007bd8
  10002c:   55                                    pushl %ebp
(nemu) si
  10002d:   89 e5                                 movl %esp,%ebp
(nemu) si
  10002f:   8b 45 08                              movl 0x8(%ebp),%eax
(nemu) si
  100032:   85 c0                                 testl %eax,%eax
(nemu) si
[jcc] ZF=1,taken=1,target=0x00100038
  100034:   74 02                                 je 100038
EIP     nemu:0x00100038 nemu:0x00100036
(nemu) si
 */