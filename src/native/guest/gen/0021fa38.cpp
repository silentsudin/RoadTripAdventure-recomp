// guest:0x0021fa38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cellVisible(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  
  lVar1 = TestSegmentInBoxWindow(param_3 + 0x100,param_3 + 0x120);
  if (lVar1 == 0) {
    lVar1 = TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x110,param_3 + 0x130);
    if (lVar1 != 0) {
      return true;
    }
    lVar1 = TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x140,param_3 + 0x150);
    if (lVar1 == 0) {
      lVar1 = TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x140,param_3 + 0x160);
      return lVar1 != 0;
    }
  }
  return true;
}


RT_GUEST_FN(0x0021fa38, guest::cellVisible)

}  // namespace guest
