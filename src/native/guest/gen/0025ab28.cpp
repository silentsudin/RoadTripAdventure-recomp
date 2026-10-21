// guest:0x0025ab28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceStandingsMenuTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(256);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xd8, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xe0, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xe8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xf0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xf8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xfc) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = (uint32_t)(int32_t)param_2;
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    uint32_t uStack_e8 = rtguest::allocFrame(8);
    uint32_t uStack_e0 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_d0 = rtguest::allocFrame(8);
    uint32_t uStack_c8 = rtguest::allocFrame(8);
    uint32_t uStack_c0 = rtguest::allocFrame(8);
    uint32_t uStack_b8 = rtguest::allocFrame(8);
    uint32_t uStack_b0 = rtguest::allocFrame(8);
    uint32_t uStack_a8 = rtguest::allocFrame(8);
uint32_t acStack_a0 = rtguest::allocFrame(64);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  undefined1 uVar1;
  byte bVar2;
  undefined4 uVar3;
  short sVar4;
  int iVar5;
  undefined8 uVar6;
  ulong uVar7;
  ulong uVar8;
  int iVar9;
uint32_t __dest;
  int iVar10;
  int iVar11;
uint32_t puVar12;
  int iVar13;
  int iVar14;
uint32_t puVar15;
  
  iVar14 = (int)param_1;
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x1c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 8))) == 0) {
    iVar11 = 0;
    uVar6 = RT_CALLRA(0x25ab68, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x25ab68); pseudocode dropped the live arg */);
    iVar9 = 0;
    do {
      iVar11 = iVar11 + 1;
      __dest = (uint32_t)(iVar5 + iVar9 + 0x8008);
      ::sprintf((char *)rtguest::G((uint32_t)(acStack_a0)), (char *)rtguest::G((uint32_t)(0x328fd0)), iVar11);
      (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(acStack_a0))));
      RT_CALLRA(0x25abc0, guest::uiMenuAddItem(uVar6,__dest,0x10));
      iVar9 = iVar11 * 0x20;
    } while (iVar11 < 5);
    puVar15 = (uint32_t)uVar6;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar15 + 0x5f))) = 2;
    (*(uint16_t*)rtguest::G((uint32_t)puVar15)) = 0x154;
    uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x80a8)));
    (*(uint16_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(1) * sizeof(uint16_t))) = 0x2c;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar15 + 0xd))) = uVar1;
    (*(uint16_t*)rtguest::G((uint32_t)puVar15 + (uint32_t)(2) * sizeof(uint16_t))) = 0x104;
  }
  RT_CALLRA(0x25ac10, guest::uiArenaInit(iVar5,0x8000));
  RT_CALLRA(0x25ac24, guest::uiArenaBeginBatch(iVar5,0,0));
  RT_CALLRA(0x25ac38, guest::uiArenaSetTexture(iVar5,0x2c3a48,1,0));
  puVar12 = (uint32_t)(iVar5 + 0x80fc);
  RT_CALLRA(0x25ac4c, guest::uiArenaDataTag(iVar5,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0x1c000000280;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e8)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_e0)) = 0;
  RT_CALLRA(0x25acc0, guest::uiArenaSpritesWrap(iVar5,uStack_100,1,0,0,1));
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28)));
  iVar13 = 4;
  RT_CALLRA(0x25acd8, guest::uiMenuRun(iVar9,(*(int32_t*)rtguest::G((uint32_t)iStack_60))));
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar12));
  iVar11 = 0x2c;
  while( true ) {
    puVar12 = puVar12 + 0x4;  // T* element step
    iVar13 = iVar13 + -1;
    RT_CALLRA(0x25acfc, guest::uiDrawTime(500,iVar11,uVar3,0));
    if (iVar13 < 0) break;
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar12));
    iVar11 = iVar11 + 0xc;
  }
  iVar13 = 0;
  RT_CALLRA(0x25ad20, guest::uiDrawPanel9(0x154,0x74,0x104,0x60,0));
  RT_CALLRA(0x25ad38, guest::DrawTextHalfY(0x168,0xf4,0x334b88,0));
  RT_CALLRA(0x25ad68, guest::uiDrawTime(500,0x7a,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x80a8))) * 4 + iVar5 + 0x8110))),0));
  iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2c3ae0u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xd)))) * 4)))));
  if (iVar11 != 0) {
    iVar10 = 0x10c;
    do {
      RT_CALLRA(0x25ada8, guest::DrawTextHalfY(0x168,iVar10,iVar11,0));
      iVar13 = iVar13 + 1;
      iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(0x2c3ae0u + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xd)))) * 4))) + iVar13 * 4)));
      iVar10 = iVar10 + 0x18;
    } while (iVar11 != 0);
  }
  RT_CALLRA(0x25ade8, guest::DrawTextHalfY(0x168,0x16c,0x329008,6));
  RT_CALLRA(0x25ae00, guest::DrawTextHalfY(0x168,0x184,0x329018,6));
  RT_CALLRA(0x25ae14, guest::uiArenaSetTexture(iVar5,0x2c3a48,0,0));
  RT_CALLRA(0x25ae20, guest::uiArenaDataTag(iVar5,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d0)) = 0x2200000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_c0)) = 0x2800000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  RT_CALLRA(0x25ae88, guest::uiArenaSprites(iVar5,uStack_d0,1,0,0,1));
  iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x80a8))) * 0x81;
  uVar7 = (uint32_t)(iVar11 + 0x3a34);
  uVar8 = (long)(iVar11 + 0x3ab4) << 0x25;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_b0)) = uVar7 | 0x21410000 | uVar8 | 0xe00000000;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_a8)) = uVar7 | 0x21310000 | uVar8 | 0x2000000e00000000;
  RT_CALLRA(0x25aef8, guest::uiArenaSetTexture(iVar5,uStack_b0,0,0));
  RT_CALLRA(0x25af04, guest::uiArenaDataTag(iVar5,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_d0)) = 0x6400000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_c0)) = 0x10000000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  RT_CALLRA(0x25af68, guest::uiArenaSprites(iVar5,uStack_d0,1,0,0,1));
  RT_CALLRA(0x25af70, guest::uiArenaSubmit(iVar5));
  bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25)));
  if (bVar2 != 1) {
    if (1 < bVar2) {
      if (bVar2 != 2) {
        return 0;
      }
      sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) + 1;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = sVar4;
      if (sVar4 < 0x11) {
        return 0;
      }
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x18)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x20)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
      RT_CALLRA(0x25b088, guest::gs_tasks_kill(param_1));
      return 0;
    }
    if (bVar2 != 0) {
      return 0;
    }
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) & 0x7f;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 1;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xd))) == '\x05') {
    iVar5 = -1;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x80a8))) = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xd)));
    iVar5 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xd)));
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x20))) = iVar5;
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x40) == 0) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x10) != 0) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) | 0x80;
      RT_CALLRA(0x25b008, guest::PlayUiSound(0x1b));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x20))) = 0xffffffff;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 2;
    }
  }
  else {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x28))) + 0xe))) | 0x80;
    RT_CALLRA(0x25b038, guest::PlayUiSound(0x1a));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x25))) = 2;
  }
  return 0;
}


RT_GUEST_FN(0x0025ab28, guest::raceStandingsMenuTask)

}  // namespace guest
