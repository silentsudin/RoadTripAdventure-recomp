// guest:0x002075f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetSpriteXY(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(0);
  short sVar1;
  int iVar2;
  
  iVar2 = (int)param_3;
  sVar1 = (param_4 + param_6) * 8;
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = param_4 * 8 + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = (short)(iVar2 << 4) + 0x6c00;
    (*(int16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int16_t))) = sVar1 + 0x7900;
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = (short)(iVar2 + param_5) * 0x10 + 0x6c00;
    return 0;
  }
  (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int16_t))) = param_4 * 8 + 0x7800;
  (*(int16_t*)rtguest::G((uint32_t)param_1)) = (short)((iVar2 << 6) / 5) + 0x7000;
  (*(int16_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int16_t))) = sVar1 + 0x7800;
  (*(int16_t*)rtguest::G((uint32_t)param_2)) = (short)(((iVar2 + param_5) * 0x40) / 5) + 0x7000;
  return 0;
}


RT_GUEST_FN(0x002075f0, guest::uiSetSpriteXY)

}  // namespace guest
