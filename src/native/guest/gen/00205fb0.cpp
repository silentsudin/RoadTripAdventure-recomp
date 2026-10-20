// guest:0x00205fb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MeasureTextWidth(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  byte bVar1;
  int iVar2;
  int iVar3;
uint32_t pbVar4;
  
  iVar2 = 0;
  pbVar4 = (uint32_t)((int)param_1 + param_2 + 0x50);
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
  if ((bVar1 & 0x40) == 0) {
    iVar2 = RT_CALLRA(0x205ff0, guest::uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 * 4 + (int)param_1 + 0x14)))));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
  }
  if ((bVar1 & 0x80) != 0) {
    iVar3 = RT_CALLRA(0x20600c, guest::MeasureTextWidth(param_1,param_2 + -1));
    iVar2 = iVar2 + iVar3;
  }
  return iVar2;
}


RT_GUEST_FN(0x00205fb0, guest::MeasureTextWidth)

}  // namespace guest
