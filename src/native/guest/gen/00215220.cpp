// guest:0x00215220 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdatePauseConfirmDialogTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(160);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
    uint32_t uStack_80 = rtguest::allocFrame(4);
    uint32_t iStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    uint32_t iStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t auStack_60 = rtguest::allocFrame(16);
  long lVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  
  iVar2 = (int)param_3;
  puVar7 = (uint32_t)param_4;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 2;
    InitMenuElementLayout(param_3,iVar2 + 0x820c,auStack_60,0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81d8))) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x81ac))) = 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0xf))) == '\0') {
    lVar1 = HandleConfirmationMenuInput(param_1,param_2,param_3,param_4);
    if (lVar1 == 299) {
      iVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t)));
      if (iVar5 == 1) {
        gs_tasks_spawn(param_1,0x20dc50,0x40018);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 9;
      }
      else if ((iVar5 < 2) && (iVar5 == 0)) {
        gs_tasks_spawn(param_1,0x20dc50,0x40017);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 7;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0xb;
      }
    }
    else if (lVar1 == 399) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 0;
    }
    iVar5 = iVar2 + 0x20;
    uiArenaInit(iVar5,0x8000);
    uiArenaBeginBatch(iVar5,0,0);
    DrawMenuGridBorder(param_3,iVar5,iVar2 + 0x81a4,1);
    DrawMenuGridBackground(param_3,iVar5,iVar2 + 0x81c8,1);
    DrawMenuCursorAndIcons(param_3,iVar5,iVar2 + 0x820c,0xb);
    uiArenaSetTexture(iVar5,0x29c030,6,0);
    uiArenaDataTag(iVar5,1);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = 0x98;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x10e000000d8;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    uiArenaSprites(iVar5,uStack_a0,1,0x78,0x68,0);
    if ('\0' < (*(int8_t*)rtguest::G(0x177fdb1))) {
      iVar2 = (int)(*(int8_t*)rtguest::G(0x177fdb1));
      uiArenaDataTag(iVar5,1);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = 0x2000000024;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x6800000098;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (uint32_t)(uint32_t)((iVar2 + -1) * 0x68) << 0x20;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      uiArenaSprites(iVar5,uStack_a0,1,0x78,0x68,0);
    }
    iVar4 = 0;
    uiArenaSetTexture(iVar5,0x29c030,7,0);
    uiArenaDataTag(iVar5,6);
    iVar6 = 0;
    iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t)));
    iVar3 = 0;
    while( true ) {
      (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = iVar4 + 0x18;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0x160;
      if (iVar2 != iVar3) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0x180;
      }
      (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0x1a;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0x38;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = 0xd8;
      (*(int32_t*)rtguest::G((uint32_t)iStack_74)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_7c)),0xd8);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_78));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x380000001a;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      uiArenaSprites(iVar5,uStack_a0,1,0x78,0x68,0);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0xb0;
      (*(int32_t*)rtguest::G((uint32_t)iStack_7c)) = iVar4 + 0x20;
      iVar4 = iVar4 + 0x54;
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t))) != iVar3) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_74)) = iVar6 << 3;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = 0xd8;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0xb0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0x28;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
      iVar6 = iVar6 + 5;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_7c)),0xd8);
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_74)),(*(uint32_t*)rtguest::G((uint32_t)uStack_78)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x28000000b0;
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      uiArenaSprites(iVar5,uStack_a0,1,0x78,0x68,0);
      if (2 < iVar3 + 1) break;
      iVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(2) * sizeof(uint32_t)));
      iVar3 = iVar3 + 1;
    }
    uiArenaSubmit(iVar5);
  }
  return 0;
}


RT_GUEST_FN(0x00215220, guest::UpdatePauseConfirmDialogTask)

}  // namespace guest
