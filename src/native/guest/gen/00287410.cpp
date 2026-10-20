// guest:0x00287410 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSendCmd(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
  _sceSifSendCmd(param_1,0,param_2,param_3,param_4,param_5,param_6);
  return 0;
}


RT_GUEST_FN(0x00287410, guest::sceSifSendCmd)

}  // namespace guest
