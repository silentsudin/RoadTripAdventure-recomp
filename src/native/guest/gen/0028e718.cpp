// guest:0x0028e718 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t getReent(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return (*(uint32_t*)rtguest::G(0x002e8cc0));
}


RT_GUEST_FN(0x0028e718, guest::getReent)

}  // namespace guest
