// guest:0x0028e4b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t __fixunsdfdi(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  long lVar1;
  long lVar2;
  undefined8 uVar3;
  ulong uVar4;
  
  lVar1 = dpcmp(param_1,0);
  lVar2 = 0;
  if (-1 < lVar1) {
    uVar3 = dpmul(param_1,0x3df0000000000000);
    uVar4 = dptoul(uVar3);
    lVar2 = uVar4 << 0x20;
    if (lVar2 < 0) {
      uVar3 = int64ToDouble((uVar4 & 0xffffffff) << 0x1f);
      uVar3 = dpadd(uVar3,uVar3);
    }
    else {
      uVar3 = int64ToDouble(lVar2);
    }
    uVar3 = dpsub(param_1,uVar3);
    lVar1 = dpcmp(uVar3,0);
    if (lVar1 < 0) {
      uVar3 = dpsub(0);
      uVar4 = dptoul(uVar3);
      uVar4 = -(uVar4 & 0xffffffff);
    }
    else {
      uVar4 = dptoul(uVar3,uVar3);
      uVar4 = uVar4 & 0xffffffff;
    }
    lVar2 = lVar2 + uVar4;
  }
  return lVar2;
}


RT_GUEST_FN(0x0028e4b8, guest::__fixunsdfdi)

}  // namespace guest
