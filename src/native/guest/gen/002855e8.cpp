// guest:0x002855e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysStubReturnNeg1(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  return 0xffffffff;
}


RT_GUEST_FN(0x002855e8, guest::sysStubReturnNeg1)

}  // namespace guest
