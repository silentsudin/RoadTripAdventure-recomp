// guest:0x00285f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iRotateThreadReadyQueue(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = (*(uint32_t*)rtguest::G(0x1776570));
  if ((param_1 < 0x80) && ((*(int32_t*)rtguest::G(0x2e98f0)) != 0)) {
    iVar2 = ((*(uint32_t*)rtguest::G(0x177657c)) & 0x1ff) * 2;
    (*(uint32_t*)rtguest::G(0x177657c)) = ((*(uint32_t*)rtguest::G(0x177657c)) & 0x1ff) + 1;
    (*(uint32_t*)rtguest::G((uint32_t)0x1776580 + (uint32_t)(iVar2) * 4)) = 1;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1776581))) = (char)param_1;
    RT_CALLRA(0x285fd0, guest::iSignalSema(uVar1));
  }
  else {
    param_1 = 0xffffffff;
  }
  return param_1;
}


RT_GUEST_FN(0x00285f70, guest::iRotateThreadReadyQueue)

}  // namespace guest
