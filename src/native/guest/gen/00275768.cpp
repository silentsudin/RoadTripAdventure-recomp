// guest:0x00275768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t __kernel_cos(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  ulong uVar6;
  
  uVar6 = param_1 >> 0x20 & 0x7fffffff;
  if ((uVar6 < 0x3e400000) && (lVar1 = dptoli(), lVar1 == 0)) {
    uVar2 = 0x3ff0000000000000;
  }
  else {
    uVar2 = dpmul(param_1,param_1);
    uVar3 = dpmul(uVar2,0xbda8fae9be8838d4);
    uVar3 = dpadd(uVar3,0x3e21ee9ebdb4b1c4);
    uVar3 = dpmul(uVar2,uVar3);
    uVar3 = dpadd(uVar3,0xbe927e4f809c52ad);
    uVar3 = dpmul(uVar2,uVar3);
    uVar3 = dpadd(uVar3,0x3efa01a019cb1590);
    uVar3 = dpmul(uVar2,uVar3);
    uVar3 = dpadd(uVar3,0xbf56c16c16c15177);
    uVar3 = dpmul(uVar2,uVar3);
    uVar3 = dpadd(uVar3,0x3fa555555555554c);
    uVar3 = dpmul(uVar2,uVar3);
    if (uVar6 < 0x3fd33333) {
      uVar4 = dpmul(uVar2,0x3fe0000000000000);
      uVar2 = dpmul(uVar2,uVar3);
      uVar3 = dpmul(param_1,param_2);
      uVar2 = dpsub(uVar2,uVar3);
      uVar2 = dpsub(uVar4,uVar2);
      uVar2 = dpsub(0x3ff0000000000000,uVar2);
    }
    else {
      lVar1 = 0x3fd2000000000000;
      if (uVar6 < 0x3fe90001) {
        lVar1 = (long)((int)uVar6 + -0x200000) << 0x20;
      }
      uVar4 = dpmul(uVar2,0x3fe0000000000000);
      uVar4 = dpsub(uVar4,lVar1);
      uVar5 = dpsub(0x3ff0000000000000,lVar1);
      uVar2 = dpmul(uVar2,uVar3);
      uVar3 = dpmul(param_1,param_2);
      uVar2 = dpsub(uVar2,uVar3);
      uVar2 = dpsub(uVar4,uVar2);
      uVar2 = dpsub(uVar5,uVar2);
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00275768, guest::__kernel_cos)

}  // namespace guest
