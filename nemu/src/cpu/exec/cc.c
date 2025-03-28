#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
      if(cpu.OF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_O");
      break;
    case CC_B:
       if(cpu.CF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_B");
      break;
    case CC_E:
       if(cpu.ZF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_E:%d\n",*dest);
      break;
    case CC_BE:
       if(cpu.CF||cpu.ZF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_BE");
      break;
    case CC_S:
       if(cpu.SF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_S");
      break;
    case CC_L:
       if(cpu.OF!=cpu.SF||cpu.ZF)
        *dest = 1;
      else
      {
        *dest = 0;
      }
      printf("CC_L");
      break;
    case CC_LE:
      *dest = cpu.ZF==1||(cpu.SF!=cpu.OF);
      printf("CC_LE");
      break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
/*
 (nemu) si
subcode:4
CC_E:1
[jcc] ZF=1,taken=1,target=0x001000a4
  100090:   74 12                                 je 1000a4
(nemu) info r
eax:0x00007bcf
ecx:0x00000002
edx:0x00007bcf
ebx:0x00007bce
esp:0x00007b94
ebp:0x00007ba8
esi:0x12fd10ab
edi:0x146a1650
(nemu) si
  1000a4:   8b 5d f8                              movl -0x8(%ebp),%ebx
(nemu) info r
eax:0x00007bcf
ecx:0x00000002
edx:0x00007bcf
ebx:0x00007bcf
esp:0x00007b94
ebp:0x00007ba8
esi:0x12fd10ab
edi:0x146a1650
(nemu) si
  1000a7:   88 c8                                 movb %cl,%al
(nemu) info r
eax:0x00007b02
ecx:0x00000002
edx:0x00007bcf
ebx:0x00007bcf
esp:0x00007b94
ebp:0x00007ba8
esi:0x12fd10ab
edi:0x146a1650
(nemu) si
  1000a9:   f7 d0                                 notl %eax
eax     nemu:0x00000000 qemu:0xffff84fd
 */