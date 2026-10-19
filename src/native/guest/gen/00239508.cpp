// guest:0x00239508 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowResidentMessage(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  long lVar4;
  uint uVar5;
uint32_t piVar6;
  int iVar7;
  int iVar8;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar2 = uiMenuCreate();
    uiMenuAddItem(uVar2,0x2ec598,0);
    uVar3 = GetSlotDataPtr((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),0);
    uiMenuAddItem(uVar2,uVar3,0x80);
    uiMenuAddItem(uVar2,0x2ec5b0,0);
    uiMenuAddItem(uVar2,0x3338a0,0x10);
    uiMenuAddItem(uVar2,0x333898,0x10);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar2 + 0x5f))) = 2;
  }
  else {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    lVar4 = uiMenuRun(iVar7);
    if (lVar4 != 0) {
      if ((lVar4 != -1) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0xd))) == '\0')) {
        for (iVar7 = 1; iVar7 < 8; iVar7 = iVar7 + 1) {
          ClearFlaggedSlotBit(iVar7,(*(uint8_t*)rtguest::G((uint32_t)0x177f76c + (uint32_t)(iVar7 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) * 0x1c) * 1)),0)
          ;
        }
        uVar5 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
        piVar6 = 0x0177ea60;
        iVar7 = 10;
        do {
          iVar7 = iVar7 + -1;
          if ((*(int32_t*)rtguest::G((uint32_t)piVar6)) == (int)0x177f760 + uVar5 * 0x1c) {
            (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 +
                              (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 2))) * 4))) +
                      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 3))) * 0x10;
            uVar5 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
          }
          piVar6 = piVar6 + 0x4;  // T* element step
        } while (-1 < iVar7);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fdd0 + uVar5 * 2 + 2))) = 0;
        iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)))));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = (uint32_t)bVar1;
        gs_tasks_kill(param_1);
        PlayUiSound(0x1a);
        return 0;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xc))) = (*(uint8_t*)rtguest::G(0x239748));
      ShowSequencedMessages(param_1,param_2);
      PlayUiSound(0x1b);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00239508, guest::ShowResidentMessage)

}  // namespace guest
