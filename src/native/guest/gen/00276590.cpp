// guest:0x00276590 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __kernel_sin(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  if ((0x3e3fffff < (param_1 >> 0x20 & 0x7fffffffU)) || (lVar1 = dptoli(), lVar1 != 0)) {
    uVar2 = dpmul(param_1,param_1);
    uVar3 = dpmul(uVar2,param_1);
    uVar4 = dpmul(uVar2,0x3de5d93a5acfd57c);
    uVar4 = dpadd(uVar4,0xbe5ae5e68a2b9ceb);
    uVar4 = dpmul(uVar2,uVar4);
    uVar4 = dpadd(uVar4,0x3ec71de357b1fe7d);
    uVar4 = dpmul(uVar2,uVar4);
    uVar4 = dpadd(uVar4,0xbf2a01a019c161d5);
    uVar4 = dpmul(uVar2,uVar4);
    uVar4 = dpadd(uVar4,0x3f8111111110f8a6);
    if (param_3 == 0) {
      uVar2 = dpmul(uVar2,uVar4);
      uVar2 = dpadd(uVar2,0xbfc5555555555549);
      uVar2 = dpmul(uVar3,uVar2);
      param_1 = dpadd(param_1,uVar2);
    }
    else {
      uVar5 = dpmul(param_2,0x3fe0000000000000);
      uVar4 = dpmul(uVar3,uVar4);
      uVar4 = dpsub(uVar5,uVar4);
      uVar2 = dpmul(uVar2,uVar4);
      uVar2 = dpsub(uVar2,param_2);
      uVar3 = dpmul(uVar3,0xbfc5555555555549);
      uVar2 = dpsub(uVar2,uVar3);
      param_1 = dpsub(param_1,uVar2);
    }
  }
  return param_1;
}


RT_GUEST_FN(0x00276590, guest::__kernel_sin)

}  // namespace guest
