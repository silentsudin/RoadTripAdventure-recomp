// guest:0x002590b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplySoundVolumes(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (*(int16_t*)rtguest::G(0x1780a3e)) * 0x3fff;
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0x7f;
  }
  SoundRpcCmd31(iVar1 >> 7,iVar1 >> 7);
  if ('\0' < (*(int8_t*)rtguest::G(0x1780a3d))) {
    SoundRpcCmd33PlaySe((*(int8_t*)rtguest::G(0x1780a3d)) + -1,param_1);
    SoundRpcCmd34();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002590b0, guest::ApplySoundVolumes)

}  // namespace guest
