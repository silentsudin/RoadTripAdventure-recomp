// guest:0x0028cab8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadElf(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  _sceSifLoadElfPart(param_1,0x3327b8,param_2,1);
  return 0;
}


RT_GUEST_FN(0x0028cab8, guest::sceSifLoadElf)

}  // namespace guest
