// guest:0x0027b540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// sig: param_1 ulong (fn/0027b540.c:13-14 (long)param_1 >> 0x20 reads the high word), uint64_t
uint32_t __floatdisf(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  undefined8 uVar1;
  undefined8 uVar2;
  
  if ((0x3ffffffffffffe < param_1 + 0x1fffffffffffff) && ((param_1 & 0x7ff) != 0)) {
    param_1 = param_1 | 0x800;
  }
  uVar1 = RT_CALLRA(0x27b594, guest::litodp((long)param_1 >> 0x20));
  uVar1 = RT_CALLRA(0x27b5a4, guest::dpmul(uVar1,0x40f0000000000000));
  uVar1 = RT_CALLRA(0x27b5bc, guest::dpmul(uVar1,0x40f0000000000000));
  uVar2 = RT_CALLRA(0x27b5d0, guest::litodp((int)param_1));
  if ((int)param_1 < 0) {
    uVar2 = RT_CALLRA(0x27b5ec, guest::dpadd(uVar2,0x41f0000000000000));
  }
  uVar1 = RT_CALLRA(0x27b5fc, guest::dpadd(uVar1,uVar2));
  RT_CALLRA(0x27b608, guest::dptofp(uVar1));
  return 0;
}


RT_GUEST_FN(0x0027b540, guest::__floatdisf)

}  // namespace guest
