// guest:0x00261de0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t roulettePayoutTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x88, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x90, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x98) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x9c) = 0x00000000u;
    rtguest::gS[0] = 0x00000000002d9948ull;  // walk const
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(7, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
uint32_t aiStack_80 = rtguest::allocFrame(20);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t uStack_58 = rtguest::allocFrame(4);
    uint32_t uStack_54 = rtguest::allocFrame(4);
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  short sVar9;
  int iVar10;
uint32_t puVar11;
uint32_t piVar12;
  int iVar13;
  int iVar14;
  
  iVar14 = (int)param_1;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)));
  RT_CALLRA(0x261e24, guest::uiArenaInit(iVar1,0x8000));
  RT_CALLRA(0x261e34, guest::uiArenaBeginBatch(iVar1,0,0));
  RT_CALLRA(0x261e48, guest::uiArenaSetTexture(iVar1,0x2d9948,0,0));
  RT_CALLRA(0x261e54, guest::uiArenaDataTag(iVar1,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_a0)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x1c000000280;
  RT_CALLRA(0x261e90, guest::uiArenaSprites(iVar1,uStack_a0,1,0,0,1));
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8020))) != 0) &&
     (iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8020))) + -1, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 8))) & 0x20) != 0)) {
    iVar13 = 2;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(1) * 4)) = iVar10 / 6 + 0x12;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8024))) + 0x15;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4)) = iVar10;
    RT_CALLRA(0x261f10, guest::uiArenaSetTexture(iVar1,0x2d9948,1,0));
    RT_CALLRA(0x261f1c, guest::uiArenaDataTag(iVar1,3));
    piVar12 = aiStack_80;
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4));
    while( true ) {
      iVar13 = iVar13 + -1;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(4) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9970 + iVar10 * 0x30)));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9974 + iVar2 * 0x30)));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9970 + iVar3 * 0x30)));
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9974 + iVar4 * 0x30)));
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9978 + iVar5 * 0x30)));
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      piVar12 = piVar12 + 0x4;  // T* element step
      (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_54)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d997c + iVar6 * 0x30)));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9974 + iVar2 * 0x30))),
                           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9970 + iVar10 * 0x30))));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9974 + iVar4 * 0x30))),
                           (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9970 + iVar3 * 0x30))));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9978 + iVar5 * 0x30))));
      (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = 0;
      RT_CALLRA(0x262040, guest::uiArenaSprites(iVar1,uStack_a0,1,0,0,1));
      if (iVar13 < 0) break;
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
    }
  }
  iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8008))) * 0x30;
  uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d996c + iVar10)));
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9968 + iVar10)));
  RT_CALLRA(0x262090, guest::uiArenaSetTexture(iVar1,0x2d9948,1,0));
  RT_CALLRA(0x26209c, guest::uiArenaDataTag(iVar1,1));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_a0)), 0, 0x20));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = 0x2800000028;
  RT_CALLRA(0x2620d4, guest::uiArenaSprites(iVar1,uStack_a0,1,uVar8,uVar7,1));
  RT_CALLRA(0x2620dc, guest::uiArenaSubmit(iVar1));
  iVar10 = (int)param_2;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)))) {
  case 0:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 1;
  case 1:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x50) == 0) {
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    else {
      RT_CALLRA(0x262128, guest::PlayUiSound(0x1a));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0x80;
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = sVar9 + 1;
    if (0x80 < sVar9) {
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 2;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8028)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 4))) = uVar7;
    }
    break;
  case 2:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x50) == 0) {
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    else {
      RT_CALLRA(0x262190, guest::PlayUiSound(0x1a));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0x80;
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = sVar9 + 1;
    if (0x80 < sVar9) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 3;
    }
    break;
  case 3:
    iVar13 = 0;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8028))) != 0) {
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8010))) * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002d9980 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8008))) * 0x30)));
    }
    puVar11 = (uint32_t)(iVar13 + (*(int32_t*)rtguest::G(0x177fdb4)));
    if (999999 < (int)puVar11) {
      puVar11 = 0x000f423f;
    }
    RT_CALLRA(0x26223c, guest::DrawScoreAndRemaining((*(int32_t*)rtguest::G(0x177fdb4)) - (int)puVar11,param_2));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x30))) & 0x50) == 0) {
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    else {
      RT_CALLRA(0x262254, guest::PlayUiSound(0x1a));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0xc0;
      sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2)));
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = sVar9 + 1;
    if (0xc0 < sVar9) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
      RT_CALLRA(0x262288, guest::SpendTimerBudget(-iVar13,0));
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 4;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 2;
    }
    break;
  case 4:
    RT_CALLRA(0x2622b4, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) << 0x18));
    sVar9 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) + 1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = sVar9;
    if (0x80 < sVar9) {
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x18)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
      RT_CALLRA(0x2622f0, guest::gs_tasks_kill(param_1));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00261de0, guest::roulettePayoutTask)

}  // namespace guest
