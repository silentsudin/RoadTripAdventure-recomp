// guest:0x00258578 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundInitVolumes(uint32_t q0, ...) {
  SoundRpcStopAll();
  SoundRpcSetVolume(0,0x1ccc,0x1ccc);
  SoundRpcSetVolume(1,0x1ccc,0x1ccc);
  return 0;
}


RT_GUEST_FN(0x00258578, guest::SoundInitVolumes)

}  // namespace guest
