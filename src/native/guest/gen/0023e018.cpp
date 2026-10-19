// guest:0x0023e018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceOscillatingValue(uint32_t param_1, ...) {
  char cVar1;
  byte bVar2;
  ushort uVar3;
  int iVar4;
uint32_t psVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
uint32_t pcVar10;
  long lVar5;
  
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1));
  uVar9 = (uint32_t)uVar3;
  iVar4 = (int)(short)uVar3;
  lVar5 = (long)iVar4;
  if (lVar5 < 1) {
    (*(int8_t*)rtguest::G(0x335708)) = 0;
  }
  bVar2 = (*(int8_t*)rtguest::G(0x335708));
  pcVar10 = 0x002a36c8;
  psVar6 = 0x002a36b0;
  iVar7 = 0;
  while( true ) {
    iVar8 = iVar7 + 1;
    if ((((*(int16_t*)rtguest::G((uint32_t)psVar6)) <= lVar5) && (lVar5 < (*(int16_t*)rtguest::G((uint32_t)psVar6 + (uint32_t)(1) * sizeof(int16_t))))) && ((*(int8_t*)rtguest::G(0x335708)) == 0)) {
      uVar3 = uVar3 + (short)(*(int8_t*)rtguest::G((uint32_t)pcVar10));
      (*(uint16_t*)rtguest::G((uint32_t)param_1)) = uVar3;
      goto LAB_0023e11c;
    }
    if ((((long)iVar4 <= (long)(short)(*(uint32_t*)rtguest::G((uint32_t)0x2a36b0 + (uint32_t)(iVar7) * 4))) &&
        ((long)(short)(*(uint32_t*)rtguest::G((uint32_t)0x2a36b2 + (uint32_t)(iVar7) * 4)) < (long)iVar4)) && ((*(int8_t*)rtguest::G(0x335708)) != 0)) break;
    pcVar10 = pcVar10 + 1;
    psVar6 = psVar6 + 1;
    iVar7 = iVar8;
    if (9 < iVar8) {
LAB_0023e11c:
      if (0x1fe < (short)uVar3) {
        (*(int8_t*)rtguest::G(0x335708)) = (*(int8_t*)rtguest::G(0x335708)) ^ 1;
      }
      return 0;
    }
  }
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar10));
  uVar3 = (uint32_t)(uVar9 - (int)cVar1);
  (*(uint16_t*)rtguest::G((uint32_t)param_1)) = uVar3;
  if ((int)((uVar9 - (int)cVar1) * 0x10000) >> 0x10 < 0x1ff) {
    return 0;
  }
  (*(int8_t*)rtguest::G(0x335708)) = bVar2 ^ 1;
  goto LAB_0023e11c;
}


RT_GUEST_FN(0x0023e018, guest::AdvanceOscillatingValue)

}  // namespace guest
