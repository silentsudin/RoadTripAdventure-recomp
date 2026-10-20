// guest:0x002855f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysLseek(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  return 0xffffffff;
}


RT_GUEST_FN(0x002855f0, guest::sysLseek)

}  // namespace guest
