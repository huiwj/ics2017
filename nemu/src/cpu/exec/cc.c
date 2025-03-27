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
  100000:   bd 00 00 00 00                        movl $0x0,%ebp
(nemu) si
  100005:   bc 00 7c 00 00                        movl $0x7c00,%esp
(nemu) si
[Call] before:EIP=0x0010000f,ESP=0x00007c00
)  10000a:   e8 0d 00 00 00                        call 10001c
(nemu) si
push:esp: esp=0x00007bf8,val=0x00000000
  10001c:   55                                    pushl %ebp
(nemu) si
  10001d:   89 e5                                 movl %esp,%ebp
(nemu) si
zf:0
  10001f:   83 ec 08                              subl $0x8,%esp
(nemu) si
[Call] before:EIP=0x00100027,ESP=0x00007bf0
)  100022:   e8 2d 00 00 00                        call 100054
(nemu) si
  100054:   8d 4c 24 04                           leal 0x4(%esp),%ecx
(nemu) si
zf:0
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
push:esp: esp=0x00007bd4,val=0x1ffd38be
  100061:   57                                    pushl %edi
(nemu) si
push:esp: esp=0x00007bd0,val=0x35beb829
  100062:   56                                    pushl %esi
(nemu) si
push:esp: esp=0x00007bcc,val=0x76fdd8bc
  100063:   53                                    pushl %ebx
(nemu) si
push:esp: esp=0x00007bc8,val=0x00007bf0
  100064:   51                                    pushl %ecx
(nemu) si
zf:0
  100065:   83 ec 08                              subl $0x8,%esp
(nemu) si
zf:1
  100068:   31 ff                                 xorl %edi,%edi
(nemu) si
  10006a:   66 90                                 nop
(nemu) si
  10006c:   8d 34 3f                              leal (%edi,%edi,1),%esi
(nemu) si
zf:1
  10006f:   31 db                                 xorl %ebx,%ebx
(nemu) si
  100071:   8d 76 00                              leal 0(%esi),%esi
(nemu) si
zf:0
  100074:   83 ec 0c                              subl $0xc,%esp
(nemu) si
  100077:   8b 87 e0 01 10 00                     movl 0x1001e0(%edi),%eax
(nemu) si
zf:1
  10007d:   03 83 e0 01 10 00                     addl 0x1001e0(%ebx),%eax
(nemu) si
zf:1
  100083:   3b 84 b3 e0 00 10 00                  cmpl 0x1000e0(%ebx,%esi,4),%eax
(nemu) info r
eax:0x00000000
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
CC_E:1
  10008a:   0f 94 c0                              sete %al
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
  10008d:   0f b6 c0                              movzxl %al,%al
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
push:esp: esp=0x00007bb0,val=0x00000001
  100090:   50                                    pushl %eax
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bb0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
[Call] before:EIP=0x00100096,ESP=0x00007bb0
)  100091:   e8 96 ff ff ff                        call 10002c
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bac
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
push:esp: esp=0x00007ba8,val=0x00007bd8
  10002c:   55                                    pushl %ebp
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007ba8
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
  10002d:   89 e5                                 movl %esp,%ebp
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007ba8
ebp:0x00007ba8
esi:0x00000000
edi:0x00000000
(nemu) si
  10002f:   8b 45 08                              movl 0x8(%ebp),%eax
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007ba8
ebp:0x00007ba8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
[test] ZF=0,SF=0
  100032:   85 c0                                 testl %eax,%eax
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007ba8
ebp:0x00007ba8
esi:0x00000000
edi:0x00000000
(nemu) si
CC_E:0
[jcc] ZF=0,taken=0,target=0x00100038
  100034:   74 02                                 je 100038
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007ba8
ebp:0x00007ba8
esi:0x00000000
edi:0x00000000
(nemu) si
  100036:   5d                                    popl %ebp
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bac
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
[Ret] pop ret=0x00007bd8,new EIP=0x00100096,new ESP=0x00100037
  100037:   c3                                    ret
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000000
esp:0x00007bb0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  100096:   83 c3 04                              addl $0x4,%ebx
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
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
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  10009c:   83 fb 20                              cmpl $0x20,%ebx
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
CC_E:0
[jcc] ZF=0,taken=1,target=0x00100074
  10009f:   75 d3                                 jne 100074
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bc0
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  100074:   83 ec 0c                              subl $0xc,%esp
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
  100077:   8b 87 e0 01 10 00                     movl 0x1001e0(%edi),%eax
(nemu) info r
eax:0x00000000
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  10007d:   03 83 e0 01 10 00                     addl 0x1001e0(%ebx),%eax
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
zf:0
  100083:   3b 84 b3 e0 00 10 00                  cmpl 0x1000e0(%ebx,%esi,4),%eax
(nemu) info r
eax:0x00000001
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
(nemu) si
CC_E:0
  10008a:   0f 94 c0                              sete %al
eax     nemu:0x00000000 qemu:0x00000001
(nemu) info r
eax:0x00000000
ecx:0x00007bf0
edx:0x549f6eaf
ebx:0x00000004
esp:0x00007bb4
ebp:0x00007bd8
esi:0x00000000
edi:0x00000000
 */