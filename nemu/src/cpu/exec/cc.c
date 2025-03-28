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
 100096:   83 c3 04                              addl $0x4,%ebx
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x64f4b578
ebx:0x00000004
esp:0x00007bb0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  100099:   83 c4 10                              addl $0x10,%esp
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x64f4b578
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:1
  10009c:   83 fb 20                              cmpl $0x20,%ebx
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x64f4b578
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
subcode:5
CC_E:1
[jcc] ZF=1,taken=0,target=0x00100074
  10009f:   75 d3                                 jne 100074
EIP     nemu:0x001000a1 nemu:0x00100074
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x64f4b578
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
 */