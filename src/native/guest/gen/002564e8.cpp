// guest:0x002564e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PaintEditorConfirmTask(uint32_t param_1, uint32_t param_2, ...) {
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
uint32_t puVar6;
  
  iVar5 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) * 0x1c;
  puVar6 = (uint32_t)((int)0x177f760 + iVar3);
  DrawPaintEditorDigits(param_1,0x175b570,puVar6);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) != '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 0) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) == '\0') {
        uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar3 + 5))) = uVar1;
      }
      else {
        iVar3 = 100;
        if ((((*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)))) & 0xffffff) == 0) {
          iVar3 = 0;
        }
        iVar4 = iVar3 + 100;
        if (((*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)))) >> 0x18 == 0) {
          iVar4 = iVar3;
        }
        SpendTimerBudget(iVar4,0);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = (*(uint8_t*)rtguest::G(0x2568d8));
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = (*(uint8_t*)rtguest::G(0x256640));
      PlayUiSound(0x1b);
    }
  }
  vifDirect(0x175b570,0x32c);
  iVar3 = 100;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)));
  if ((uVar2 & 0xffffff) == 0) {
    iVar3 = 0;
  }
  iVar5 = iVar3 + 100;
  if (uVar2 >> 0x18 == 0) {
    iVar5 = iVar3;
  }
  DrawScoreAndRemaining(iVar5,param_2);
  return 0;
}


RT_GUEST_FN(0x002564e8, guest::PaintEditorConfirmTask)

}  // namespace guest
