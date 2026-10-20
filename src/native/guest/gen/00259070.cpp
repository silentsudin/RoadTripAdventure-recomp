// guest:0x00259070 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundStopVoiceByHandle(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if ((*(uint32_t*)rtguest::G((uint32_t)0x335728 + (uint32_t)(param_1) * 4)) != 0) {
    SendRpcCmd9Value((*(uint32_t*)rtguest::G((uint32_t)0x335728 + (uint32_t)(param_1) * 4)));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00259070, guest::SoundStopVoiceByHandle)

}  // namespace guest
