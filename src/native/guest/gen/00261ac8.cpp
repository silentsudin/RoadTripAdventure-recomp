// guest:0x00261ac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t rouletteBetPickerTask(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_40 = rtguest::allocFrame(4);
  ushort uVar1;
  int iVar2;
  undefined4 uVar3;
  short sVar4;
  int iVar5;
  undefined4 uVar6;
  char cVar7;
  int iVar8;
uint32_t auStack_50 = rtguest::allocFrame(16);
  
  iVar8 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c)));
  uiArenaInit(iVar2,0x8000);
  uiArenaBeginBatch(iVar2,0,0);
  uiArenaSetTexture(iVar2,0x2d9948,0,0);
  uiArenaDataTag(iVar2,1);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_50)), 0, 0x20));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) = 0x280;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0x1c0;
  uiArenaSprites(iVar2,auStack_50,1,0,0,1);
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) != '\x02') || (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) & 0x10) != 0)) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8008))) * 0x30;
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9968 + iVar5)));
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d996c + iVar5)));
    uiArenaSetTexture(iVar2,0x2d9948,1,0);
    uiArenaDataTag(iVar2,1);
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_50)), 0, 0x20));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0x28;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) = 0x28;
    uiArenaSprites(iVar2,auStack_50,1,uVar6,uVar3,1);
  }
  uiArenaSubmit(iVar2);
  cVar7 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
  switch(cVar7) {
  case '\0':
    if ((*(int32_t*)rtguest::G(0x177fdb4)) < 10) {
      gs_tasks_spawn(param_1,0x2624b8,0);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 1;
    }
    else {
LAB_00261d3c:
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 2;
    }
    break;
  case '\x01':
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) == '\0') {
      cVar7 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
      goto switchD_00261c40_default;
    }
LAB_00261d00:
    PlayBgmSlot5();
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0xffffffff;
    break;
  case '\x02':
    uVar6 = navigateRouletteTargetGrid(param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8008))));
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8008))) = uVar6;
    if ((uVar1 & 0x40) == 0) {
      if ((uVar1 & 0x10) == 0) {
        cVar7 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
        goto switchD_00261c40_default;
      }
      PlayUiSound(0x1b);
      goto LAB_00261d00;
    }
    PlayUiSound(0x1a);
    gs_tasks_spawn(param_1,0x261638,iVar2);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 3;
    break;
  case '\x03':
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) != '\0') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) == -1) goto LAB_00261d3c;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25))) = 4;
    }
    break;
  case '\x04':
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x19);
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) + 1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = sVar4;
    if (0x40 < sVar4) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x20)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      gs_tasks_kill(param_1);
      return 0;
    }
    break;
  default:
    goto switchD_00261c40_default;
  }
  cVar7 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x25)));
switchD_00261c40_default:
  if (cVar7 != '\x03') {
    DrawScoreCounter();
  }
  return 0;
}


RT_GUEST_FN(0x00261ac8, guest::rouletteBetPickerTask)

}  // namespace guest
