// guest:0x00205d00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdjustWidgetCentering(uint32_t param_1, uint32_t param_2, ...) {
  short sVar1;
  int iVar2;
  int iVar3;
  
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x11))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x11))) + '\x01';
  iVar2 = uiTextWidth(param_2);
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t)));
  iVar2 = iVar2 + 0x14;
  iVar3 = (int)sVar1;
  if ((long)sVar1 < (long)iVar2) {
    sVar1 = (short)((iVar2 - sVar1) / 2);
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int16_t))) = (short)iVar2;
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = (*(int16_t*)rtguest::G((uint32_t)param_1)) - sVar1;
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) + sVar1;
    iVar3 = iVar2;
  }
  iVar3 = ((short)iVar3 - iVar2) / 2;
  if ((long)iVar3 < (long)(*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t)))) {
    (*(int16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int16_t))) = (short)iVar3;
  }
  return 0;
}


RT_GUEST_FN(0x00205d00, guest::AdjustWidgetCentering)

}  // namespace guest
