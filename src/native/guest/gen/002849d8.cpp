// guest:0x002849d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _close_r(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  
  (*(int32_t*)rtguest::G(0x33573c)) = 0;
  lVar1 = sysClose(param_2);
  if ((lVar1 == -1) && ((*(int32_t*)rtguest::G(0x33573c)) != 0)) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (*(int32_t*)rtguest::G(0x33573c));
  }
  return 0;
}


RT_GUEST_FN(0x002849d8, guest::_close_r)

}  // namespace guest
