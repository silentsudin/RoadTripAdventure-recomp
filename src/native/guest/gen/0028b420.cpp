// guest:0x0028b420 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceUmount(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  _sceCallCode(param_1,0x15);
  return 0;
}


RT_GUEST_FN(0x0028b420, guest::sceUmount)

}  // namespace guest
