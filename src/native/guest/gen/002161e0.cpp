// guest:0x002161e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateGarageMenuTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(2480);
uint32_t puVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  long lVar5;
  undefined8 uVar6;
uint32_t ppuVar7;
uint32_t puVar8;
uint32_t puVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
uint32_t piVar14;
uint32_t ppuVar15;
uint32_t apuStack_9b0 = rtguest::allocFrame(1024);
uint32_t apuStack_5b0 = rtguest::allocFrame(1024);
uint32_t auStack_1b0 = rtguest::allocFrame(80);
uint32_t auStack_160 = rtguest::allocFrame(192);
uint32_t auStack_a0 = rtguest::allocFrame(64);
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  iVar10 = 0;
  while( true ) {
    lVar5 = CallFindFlaggedSlot(0xf,0,iVar10);
    ppuVar7 = apuStack_5b0 + iVar10;
    ppuVar15 = apuStack_9b0 + iVar10;
    if (lVar5 == -1) break;
    iVar10 = iVar10 + 1;
    puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(0x2f0814u + (uint32_t)((int)lVar5 * 2) * 4)));
    (*(uint32_t*)rtguest::G((uint32_t)ppuVar15)) = (*(uint32_t*)rtguest::G((uint32_t)(0x2f0810u + (uint32_t)((int)lVar5 * 2) * 4)));
    (*(uint32_t*)rtguest::G((uint32_t)ppuVar7)) = puVar1;
  }
  iVar13 = (int)param_3;
  if (iVar10 == 0) {
    iVar10 = 1;
    (*(uint32_t*)rtguest::G((uint32_t)apuStack_5b0 + (uint32_t)(0) * 4)) = (*(uint32_t*)rtguest::G(0x003334ec));
    (*(uint32_t*)rtguest::G((uint32_t)apuStack_9b0 + (uint32_t)(0) * 4)) = (*(uint32_t*)rtguest::G(0x003334e8));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8)));
  }
  else {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8)));
  }
  if (iVar3 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * 4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 4;
    InitMenuElementLayout(param_3,iVar13 + 0x820c,auStack_60,0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81d8))) = 4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81d4))) = 0xfffff0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81cc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81d0))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81c8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81c8))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81a4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81a4))) | 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81b0))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81a8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x81ac))) = 0;
    uVar6 = uiMenuCreate(param_1);
    LayoutTextItems(uVar6,apuStack_9b0,iVar10,8);
    puVar8 = (uint32_t)uVar6;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0x5f))) = 2;
    (*(uint16_t*)rtguest::G((uint32_t)puVar8)) = 0x28;
    (*(uint16_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint16_t))) = 0x2c;
    (*(uint16_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(2) * sizeof(uint16_t))) = 0x104;
    (*(uint16_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(3) * sizeof(uint16_t))) = 0x60;
  }
  lVar5 = HandleToggleMenuInput(param_1,param_2,param_3,param_4);
  if (lVar5 == 399) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))) = 0;
  }
  iVar10 = iVar13 + 0x20;
  puVar9 = (uint32_t)(iVar13 + 0x81dc);
  uiArenaInit(iVar10,0x8000);
  uiArenaBeginBatch(iVar10,0,0);
  iVar3 = 1;
  DrawMenuGridBorder(param_3,iVar10,iVar13 + 0x81a4,1);
  DrawMenuListItems(param_3,iVar10,iVar13 + 0x81b4,1,param_2);
  DrawMenuGridBackground(param_3,iVar10,iVar13 + 0x81c8,1);
  do {
    if (((((*(uint32_t*)rtguest::G((uint32_t)puVar9)) & 1) != 0) && ((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(4) * sizeof(uint32_t))) != 0)) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 8))) & 0x10) != 0)) {
      uiArenaSetTexture(iVar10,0x29c030,0,0);
      uiArenaDataTag(iVar10,1);
      uiArenaSprites(iVar10,(*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(5) * sizeof(uint32_t))) * 0x20 + 0x29c300,1,(*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(1) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(3) * sizeof(uint32_t))));
    }
    iVar3 = iVar3 + -1;
    puVar9 = puVar9 + 0x18;  // T* element step
  } while (iVar3 != -1);
  DrawMenuCursorAndIcons(param_3,iVar10,iVar13 + 0x820c,0xb);
  uiArenaSubmit(iVar10);
  uiDrawPanel9(0x28,0x98,0x104,0x30,0);
  uiDrawPanel9(0x154,0x2c,0x104,0x9c,0);
  iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x28)));
  uiMenuWindow(iVar10,param_2,apuStack_9b0);
  ::strlen((char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)apuStack_5b0 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0xd)))) * 4)))));
  (*(uint16_t*)rtguest::G((uint32_t)auStack_160 + (uint32_t)(0) * 2)) = (*(uint32_t*)rtguest::G(0x333570));
  (uint32_t)(uintptr_t)(::strcat((char *)rtguest::G((uint32_t)((uint32_t)auStack_160)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)apuStack_5b0 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0xd)))) * 4))))));
  iVar10 = 0;
  while( true ) {
    lVar5 = ExtractDelimitedToken(auStack_a0,auStack_160,0x333570,iVar10);
    if (lVar5 == 0) break;
    DrawTextHalfY(0x28,iVar10 * 0x18 + 0x130,auStack_a0,0);
    iVar10 = iVar10 + 1;
  }
  DrawSlotScoreNumber(0x186,0x18,0);
  DrawTextHalfY(0x168,0x58,0x2eaca8,0);
  iVar10 = CountSetBitsInByteArray(0x177fca8,0x10);
  uiDrawNumber10(0x244,0x5a,100 - iVar10,0);
  piVar14 = 0x0177fdb8;
  bVar2 = false;
  uVar11 = 0;
  ppuVar15 = 0x29c51cu;
  iVar10 = 0x70;
  iVar13 = 0x72;
  do {
    iVar3 = (*(uint32_t*)rtguest::G(0x177fdb8));
    puVar1 = (*(uint32_t*)rtguest::G(0x0029c518));
    if (uVar11 == 0) {
LAB_00216674:
      iVar12 = iVar10 + 0x16;
      DrawTextHalfY(0x168,iVar13,puVar1,0);
      iVar13 = iVar13 + 0x2c;
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_1b0)), (char *)rtguest::G((uint32_t)(0x333578)));
      iVar4 = uiTextWidth(auStack_1b0);
      DrawTextHalfY(0x244 - iVar4,iVar12,auStack_1b0,0);
      iVar4 = uiTextWidth(auStack_1b0);
      iVar10 = iVar10 + 0x2c;
      uiDrawNumber10(0x244 - iVar4,iVar12,iVar3 / 0x649,0);
    }
    else {
      lVar5 = GetFlaggedSlotCount(0xb,uVar11 + 3,0);
      if (lVar5 != 0) {
        if (bVar2) {
          puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar15));
        }
        else {
          bVar2 = true;
          iVar13 = iVar13 + 0x16;
          DrawTextHalfY(0x168,iVar10,(*(uint32_t*)rtguest::G(0x0029c51c)),0);
          iVar10 = iVar10 + 0x16;
          puVar1 = (*(uint32_t*)rtguest::G((uint32_t)ppuVar15));
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar14));
        goto LAB_00216674;
      }
    }
    uVar11 = uVar11 + 1;
    piVar14 = piVar14 + 0x4;  // T* element step
    ppuVar15 = ppuVar15 + 1;
    if (5 < uVar11) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x002161e0, guest::UpdateGarageMenuTask)

}  // namespace guest
