// guest:0x00204f58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClearTaskPoolFlag(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t pbVar1;
  int iVar2;
  
  iVar2 = 0x2f;
  pbVar1 = 0x177a0d0;
  do {
    if (((*(int8_t*)rtguest::G((uint32_t)pbVar1)) & 2) != 0) {
      (*(int8_t*)rtguest::G((uint32_t)pbVar1)) = (*(int8_t*)rtguest::G((uint32_t)pbVar1)) & 0xfd;
    }
    iVar2 = iVar2 + -1;
    pbVar1 = pbVar1 + 0x30;
  } while (iVar2 != -1);
  return 0;
}


RT_GUEST_FN(0x00204f58, guest::ClearTaskPoolFlag)

}  // namespace guest
