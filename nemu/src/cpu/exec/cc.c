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
 [jump] : EIP = 100040,JMP_EIP = 100010
  100040:   e9 cb ff ff ff                        jmp 100010
(nemu) info r
eax:0x00000000
ecx:0x00000080
edx:0x00007bcf
ebx:0x00007bce
esp:0x00007bac
ebp:0x00007bd8
esi:0x1919a4f7
edi:0x3c5bb40c
(nemu) si
push:esp: esp=0x00007ba8,val=0x00007bd8
  100010:   55                                    pushl %ebp
(nemu) info r
eax:0x00000000
ecx:0x00000080
edx:0x00007bcf
ebx:0x00007bce
esp:0x00007ba8
ebp:0x00007bd8
esi:0x1919a4f7
edi:0x3c5bb40c
(nemu) si
  100011:   89 e5                                 movl %esp,%ebp
(nemu) info r
eax:0x00000000
ecx:0x00000080
edx:0x00007bcf
ebx:0x00007bce
esp:0x00007ba8
ebp:0x00007ba8
esi:0x1919a4f7
edi:0x3c5bb40c
(nemu) si
  100013:   8b 45 08                              movl 0x8(%ebp),%eax
(nemu) info r
eax:0x00000001
ecx:0x00000080
edx:0x00007bcf
ebx:0x00007bce
esp:0x00007ba8
ebp:0x00007ba8
esi:0x1919a4f7
edi:0x3c5bb40c
(nemu) si
nemu: HIT BAD TRAP at eip = 0x00100016

  100016:   d6                                    nemu trap (eax = 1)
 */