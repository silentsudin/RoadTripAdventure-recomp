// guest:0x0025bb08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawTextEntrySlots(uint32_t param_1, ...) {
  short sVar1;
  bool bVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  int iVar7;
  int iVar8;
uint32_t puVar9;
  long lVar10;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  lVar10 = 0;
  iVar4 = uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc + 0x2c40d0))));
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c40d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc)));
  iVar8 = iVar7 * 0x12;
  iVar5 = iVar4;
  if (iVar4 <= iVar8) {
    iVar5 = iVar8;
  }
  uiDrawPanel9((0x280 - (iVar5 + 0x12)) / 2,0x24,iVar5 + 0x12,0x18,0);
  uiDrawText((0x280 - iVar4) / 2,0x24,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc + 0x2c40d0))),0);
  uVar3 = (*(uint16_t*)rtguest::G(0x334c40));
  if (0 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c40d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc)))) {
    iVar7 = iVar7 * -0x12 + 0x280 >> 1;
    puVar9 = 0x01760880;
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
    while( true ) {
      bVar2 = lVar10 < sVar1;
      lVar10 = (long)((int)lVar10 + 1);
      if (bVar2) {
        uVar6 = 10;
        (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_60)), (char *)rtguest::G((uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar9))))));
      }
      else {
        (*(uint16_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 2)) = uVar3;
        uVar6 = 0;
      }
      puVar9 = puVar9 + 0x4;  // T* element step
      iVar5 = uiTextWidth(auStack_60);
      iVar5 = iVar7 - iVar5 / 2;
      iVar7 = iVar7 + 0x12;
      uiDrawText(iVar5 + 9,0x30,auStack_60,uVar6);
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c40d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc))) <= lVar10) break;
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x0025bb08, guest::drawTextEntrySlots)

}  // namespace guest
