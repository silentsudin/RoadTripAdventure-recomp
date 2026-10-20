// guest:0x002590b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplySoundVolumes(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = (*(int16_t*)rtguest::G(0x1780a3e)) * 0x3fff;
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0x7f;
  }
  RT_CALLRA(0x2590ec, guest::SoundRpcCmd31(iVar1 >> 7,iVar1 >> 7));
  if ('\0' < (*(int8_t*)rtguest::G(0x1780a3d))) {
    RT_CALLRA(0x259104, guest::SoundRpcCmd33PlaySe((*(int8_t*)rtguest::G(0x1780a3d)) + -1,param_1));
    SoundRpcCmd34();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002590b0, guest::ApplySoundVolumes)

}  // namespace guest
