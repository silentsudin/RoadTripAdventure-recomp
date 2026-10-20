// guest:0x0028a2d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceRmdir(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  _sceCallCode(param_1,8);
  return 0;
}


RT_GUEST_FN(0x0028a2d0, guest::sceRmdir)

}  // namespace guest
