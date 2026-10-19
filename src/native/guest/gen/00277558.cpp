// guest:0x00277558 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t scalbn(uint64_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  int iVar2;
  int iVar3;
  ulong uVar4;
  undefined8 uVar5;
  
  uVar4 = (long)param_1 >> 0x20;
  iVar2 = (int)((uint32_t)(param_1 >> 0x20) & 0x7ff00000) >> 0x14;
  if (iVar2 == 0) {
    if ((int)param_1 == 0 && (uVar4 & 0x7fffffff) == 0) {
      return param_1;
    }
    param_1 = dpmul(param_1,0x4350000000000000);
    uVar4 = (long)param_1 >> 0x20;
    iVar2 = ((int)((uint32_t)(param_1 >> 0x20) & 0x7ff00000) >> 0x14) + -0x36;
    if (param_2 < -50000) {
      uVar4 = dpmul(param_1,0x1a56e1fc2f8f359);
      return uVar4;
    }
  }
  iVar3 = iVar2 + param_2;
  if (iVar2 == 0x7ff) {
    uVar4 = dpadd(param_1,param_1);
    return uVar4;
  }
  if (iVar3 < 0x7ff) {
    if (0 < iVar3) {
      return param_1 & 0xffffffff | (uVar4 & 0xffffffff800fffff | (long)(iVar3 * 0x100000)) << 0x20;
    }
    if (-0x36 < iVar3) {
      uVar4 = dpmul(param_1 & 0xffffffff |
                    (uVar4 & 0xffffffff800fffff | (long)((iVar3 + 0x36) * 0x100000)) << 0x20,
                    0x3c90000000000000);
      return uVar4;
    }
    if (param_2 < 0xc351) {
      uVar5 = 0x1a56e1fc2f8f359;
    }
    else {
      uVar5 = 0x7e37e43c8800759c;
    }
  }
  else {
    uVar5 = 0x7e37e43c8800759c;
  }
  uVar1 = copysign(uVar5,param_1);
  uVar4 = dpmul(uVar1,uVar5);
  return uVar4;
}


RT_GUEST_FN(0x00277558, guest::scalbn)

}  // namespace guest
