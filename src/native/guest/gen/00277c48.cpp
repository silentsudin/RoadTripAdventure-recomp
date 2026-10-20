// guest:0x00277c48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CB_DelayTh(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(16);
  iSignalSema(param_3);
  SYNC(0);
  EI();
  return 0;
}


RT_GUEST_FN(0x00277c48, guest::CB_DelayTh)

}  // namespace guest
