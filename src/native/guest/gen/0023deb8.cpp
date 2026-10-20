// guest:0x0023deb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t IsMapEntryEnabled(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if ((param_1 != 6) && (param_1 < 0x17)) {
    return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 1 << ((int)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2a2b58 + (uint32_t)(param_1) * 4)) & 0x1fU)) != 0;
  }
  return true;
}


RT_GUEST_FN(0x0023deb8, guest::IsMapEntryEnabled)

}  // namespace guest
