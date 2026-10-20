// guest:0x00258ff0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundStopAllChannels(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
uint32_t piVar1;
  int iVar2;
  
  SoundRpcCmd25(0);
  SoundRpcCmd25(1);
  MusicRpcCmd15(1);
  piVar1 = 0x00335728;
  iVar2 = 1;
  do {
    if ((*(int32_t*)rtguest::G((uint32_t)piVar1)) != 0) {
      (*(int32_t*)rtguest::G((uint32_t)piVar1)) = 0;
    }
    iVar2 = iVar2 + -1;
    piVar1 = piVar1 + 0x4;  // T* element step
  } while (-1 < iVar2);
  return 0;
}


RT_GUEST_FN(0x00258ff0, guest::SoundStopAllChannels)

}  // namespace guest
