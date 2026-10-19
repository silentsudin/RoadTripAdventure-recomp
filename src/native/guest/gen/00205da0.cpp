// guest:0x00205da0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LayoutTextItems(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  char cVar1;
  short sVar2;
  undefined4 uVar3;
  int iVar4;
uint32_t psVar5;
uint32_t puVar6;
  int iVar7;
  
  if (param_3 < param_4) {
    param_4 = param_3;
  }
  iVar4 = 0;
  if (0 < param_4) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
    puVar6 = param_2;
    iVar7 = param_4;
    while( true ) {
      puVar6 = puVar6 + 1;
      iVar7 = iVar7 + -1;
      uiMenuAddItem(param_1,uVar3,0x10);
      iVar4 = param_4;
      if (iVar7 == 0) break;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
    }
  }
  psVar5 = (uint32_t)param_1;
  if (iVar4 < param_3) {
    param_3 = param_3 - iVar4;
    param_2 = param_2 + iVar4;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11)));
    while( true ) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
      param_2 = param_2 + 1;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11))) = cVar1 + '\x01';
      iVar7 = uiTextWidth(uVar3);
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t)));
      iVar7 = iVar7 + 0x14;
      iVar4 = (int)sVar2;
      if ((long)sVar2 < (long)iVar7) {
        sVar2 = (short)((iVar7 - sVar2) / 2);
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(2) * sizeof(int16_t))) = (short)iVar7;
        (*(int16_t*)rtguest::G((uint32_t)psVar5)) = (*(int16_t*)rtguest::G((uint32_t)psVar5)) - sVar2;
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) + sVar2;
        iVar4 = iVar7;
      }
      iVar4 = ((short)iVar4 - iVar7) / 2;
      if ((long)iVar4 < (long)(*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t)))) {
        (*(int16_t*)rtguest::G((uint32_t)psVar5 + (uint32_t)(4) * sizeof(int16_t))) = (short)iVar4;
      }
      param_3 = param_3 + -1;
      if (param_3 == 0) break;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar5 + 0x11)));
    }
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(psVar5 + 6))) = 4;
  return 0;
}


RT_GUEST_FN(0x00205da0, guest::LayoutTextItems)

}  // namespace guest
