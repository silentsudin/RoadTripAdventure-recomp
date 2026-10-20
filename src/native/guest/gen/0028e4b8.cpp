// guest:0x0028e4b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t __fixunsdfdi(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  long lVar1;
  long lVar2;
  undefined8 uVar3;
  ulong uVar4;
  
  lVar1 = RT_CALLRA(0x28e4d4, guest::dpcmp(param_1,0));
  lVar2 = 0;
  if (-1 < lVar1) {
    uVar3 = RT_CALLRA(0x28e4f0, guest::dpmul(param_1,0x3df0000000000000));
    uVar4 = RT_CALLRA(0x28e4fc, guest::dptoul(uVar3));
    lVar2 = uVar4 << 0x20;
    if (lVar2 < 0) {
      uVar3 = RT_CALLRA(0x28e518, guest::int64ToDouble((uVar4 & 0xffffffff) << 0x1f));
      uVar3 = RT_CALLRA(0x28e538, guest::dpadd(uVar3,uVar3));
    }
    else {
      uVar3 = RT_CALLRA(0x28e528, guest::int64ToDouble(lVar2));
    }
    uVar3 = RT_CALLRA(0x28e548, guest::dpsub(param_1,uVar3));
    lVar1 = RT_CALLRA(0x28e560, guest::dpcmp(uVar3,0));
    if (lVar1 < 0) {
      uVar3 = RT_CALLRA(0x28e57c, guest::dpsub(0));
      uVar4 = RT_CALLRA(0x28e588, guest::dptoul(uVar3));
      uVar4 = -(uVar4 & 0xffffffff);
    }
    else {
      uVar4 = RT_CALLRA(0x28e5a0, guest::dptoul(uVar3,uVar3));
      uVar4 = uVar4 & 0xffffffff;
    }
    lVar2 = lVar2 + uVar4;
  }
  return lVar2;
}


RT_GUEST_FN(0x0028e4b8, guest::__fixunsdfdi)

}  // namespace guest
