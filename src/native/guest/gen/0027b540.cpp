// guest:0x0027b540 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __floatdisf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined8 uVar1;
  undefined8 uVar2;
  
  if ((0x3ffffffffffffe < param_1 + 0x1fffffffffffff) && ((param_1 & 0x7ff) != 0)) {
    param_1 = param_1 | 0x800;
  }
  uVar1 = litodp((long)param_1 >> 0x20);
  uVar1 = dpmul(uVar1,0x40f0000000000000);
  uVar1 = dpmul(uVar1,0x40f0000000000000);
  uVar2 = litodp((int)param_1);
  if ((int)param_1 < 0) {
    uVar2 = dpadd(uVar2,0x41f0000000000000);
  }
  uVar1 = dpadd(uVar1,uVar2);
  dptofp(uVar1);
  return 0;
}


RT_GUEST_FN(0x0027b540, guest::__floatdisf)

}  // namespace guest
