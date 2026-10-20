// guest:0x0025bcf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEntryDialogTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t __dest;
  long lVar1;
uint32_t __src;
uint32_t puVar2;
  int iVar3;
  uint32_t uVar4;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) < 0x10) {
    uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) << 3);
  }
  else {
    uVar4 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) << 3);  // 25bd24 sll $4,$3,3
    textEntryMoveCursor(uVar4);
    lVar1 = textEntrySelectInput(param_1,param_2);
    if (lVar1 != 0) {
      __dest = (*(uint32_t*)rtguest::G((uint32_t)(0x2c40d4u + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) * 3) * 4)));
      (*(int8_t*)rtguest::G((uint32_t)__dest)) = (*(int8_t*)rtguest::G(0x334e80));
      lVar1 = 0;
      if (0 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2)))) {
        puVar2 = 0x01760880;
        __src = (*(uint32_t*)rtguest::G(0x1760880));
        while( true ) {
          puVar2 = puVar2 + 0x4;  // T* element step
          lVar1 = (long)((int)lVar1 + 1);
          (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(__src))));
          if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) <= lVar1) break;
          __src = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2));
        }
      }
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) == 0) {
        (*(uint32_t*)rtguest::G(0x1782b98)) = CONCAT44(CONCAT31(CONCAT21(CONCAT11((*(int8_t*)rtguest::G((uint32_t)__dest)),(*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(1) * sizeof(int8_t)))),(*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(2) * sizeof(int8_t)))),(*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(3) * sizeof(int8_t)))),
                                CONCAT13((*(int8_t*)rtguest::G((uint32_t)__dest + (uint32_t)(4) * sizeof(int8_t))),(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2))));
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x25be30;
    }
  }
  drawTextEntryGrid(param_1);
  drawTextEntrySlots(param_1);
  uiDrawHintBar(1);
  return 0;
}


RT_GUEST_FN(0x0025bcf0, guest::textEntryDialogTask)

}  // namespace guest
