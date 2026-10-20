// guest:0x00288e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceFsReset(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  (*(uint32_t*)rtguest::G(0x2e9988)) = 0;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01779ba8)), 0, 4));
  return 0;
}


RT_GUEST_FN(0x00288e88, guest::sceFsReset)

}  // namespace guest
