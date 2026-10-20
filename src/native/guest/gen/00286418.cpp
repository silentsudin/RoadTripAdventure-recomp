// guest:0x00286418 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyRead(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = iVar3;
  if (0 < param_2) {
    do {
      iVar3 = iVar2 + 1;
      do {
      } while ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1776aa8)) + 4))) == 0);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar2))) = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1776aa8)) + 8)))));
      ttyRingPop((*(int32_t*)rtguest::G(0x1776aa8)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar2)));
    } while (((cVar1 != '\n') && (cVar1 != '\r')) && (iVar2 = iVar3, iVar3 < param_2));
  }
  return iVar3;
}


RT_GUEST_FN(0x00286418, guest::sceTtyRead)

}  // namespace guest
