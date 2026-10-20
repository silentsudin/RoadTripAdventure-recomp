// guest:0x002236a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UploadCarAt16Env(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  vifUploadAt16(0x177f5e0);
  vifDirect(0x29d290,4);
  return 0;
}


RT_GUEST_FN(0x002236a8, guest::UploadCarAt16Env)

}  // namespace guest
