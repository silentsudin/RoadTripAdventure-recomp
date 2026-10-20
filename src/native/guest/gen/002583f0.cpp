// guest:0x002583f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetVolumeDgaRegs(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  sceSdRemote(1,0x8010,0x981,param_1);
  sceSdRemote(1,0x8010,0xa81,param_2);
  return 0;
}


RT_GUEST_FN(0x002583f0, guest::SetVolumeDgaRegs)

}  // namespace guest
