// guest:0x00252cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawGolfScorecard(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(304);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xe0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xe8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xf8, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x108, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x110, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x120, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = (uint32_t)(int32_t)param_2;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xf0, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x100, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x118, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x128) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x12c) = 0x00000000u;
    rtguest::gS[0] = 0x00000000002ac330ull;  // walk const
    rtguest::gS[3] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[6] = 0x00000000000000b4ull;  // walk const
    rtguest::gS[8] = 0x0000000000330000ull;  // walk const
  }
    uint32_t uStack_130 = rtguest::allocFrame(8);
    uint32_t uStack_128 = rtguest::allocFrame(8);
    uint32_t uStack_120 = rtguest::allocFrame(8);
    uint32_t uStack_118 = rtguest::allocFrame(8);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_108 = rtguest::allocFrame(8);
    uint32_t uStack_100 = rtguest::allocFrame(8);
    uint32_t uStack_f8 = rtguest::allocFrame(8);
    uint32_t uStack_f0 = rtguest::allocFrame(8);
    rtguest::allocFrame(24);  // guest frame gap
uint32_t acStack_d0 = rtguest::allocFrame(16);
uint32_t apuStack_c0 = rtguest::allocFrame(32);
uint32_t apuStack_a0 = rtguest::allocFrame(16);
uint32_t auStack_90 = rtguest::allocFrame(16);
uint32_t apuStack_80 = rtguest::allocFrame(16);
uint32_t auStack_70 = rtguest::allocFrame(16);
    uint32_t iStack_60 = rtguest::allocFrame(4);
    uint32_t iStack_5c = rtguest::allocFrame(4);
  byte bVar1;
uint32_t puVar2;
uint32_t puVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  short sVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  size_t sVar10;
  int iVar11;
  int iVar12;
uint32_t piVar13;
  undefined8 uVar14;
  uint uVar15;
uint32_t piVar16;
  int iVar17;
  
  iVar17 = (int)param_1;
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x1c)));
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_2;
  RT_CALLRA(0x252d00, guest::uiArenaInit(iVar9,0x8000));
  RT_CALLRA(0x252d10, guest::uiArenaBeginBatch(iVar9,0,0));
  RT_CALLRA(0x252d24, guest::uiArenaSetTexture(iVar9,0x2ac330,1,0));
  RT_CALLRA(0x252d30, guest::uiArenaDataTag(iVar9,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_100)) = 0x1c000000280;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f8)) = 0x4000000040;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_f0)) = 0;
  RT_CALLRA(0x252da8, guest::uiArenaSpritesWrap(iVar9,uStack_110,1,0,0,1));
  RT_CALLRA(0x252dbc, guest::uiArenaSetTexture(iVar9,0x2ac330,0,0));
  RT_CALLRA(0x252dc8, guest::uiArenaDataTag(iVar9,1));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_130)) = 0x2200000032;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_128)) = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0x2800000100;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0;
  RT_CALLRA(0x252e2c, guest::uiArenaSprites(iVar9,uStack_130,1,0,0,1));
  RT_CALLRA(0x252e34, guest::uiArenaSubmit(iVar9));
  RT_CALLRA(0x252e4c, guest::uiDrawPanel9(100,0x38,0x1b8,0x90,0));
  RT_CALLRA(0x252e64, guest::DrawTextHalfY(0x78,0x70,0x328378,0));
  RT_CALLRA(0x252e7c, guest::DrawTextHalfY(0x78,0x178,0x334528,0));
  iVar7 = 1;
  iVar11 = 0;
  do {
    iVar12 = iVar7;
    ::sprintf((char *)rtguest::G((uint32_t)(acStack_d0)), (char *)rtguest::G((uint32_t)(0x334510)), iVar12);
    sVar10 = ::strlen((char *)rtguest::G((uint32_t)(acStack_d0)));
    RT_CALLRA(0x252ec8, guest::DrawTextHalfY((int)sVar10 * -0x10 + 0xb4,iVar11 * 0x18 + 0x88,acStack_d0,0));
    puVar2 = (*(uint32_t*)rtguest::G(0x00334508));
    iVar7 = iVar12 + 1;
    iVar11 = iVar12;
  } while (iVar12 < 9);
  (*(int32_t*)rtguest::G((uint32_t)iStack_5c)) = iVar9 + 0xc;
  uVar15 = 0;
  piVar13 = (uint32_t)(iVar9 + 0x800c);
  uVar8 = 9;
  do {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar13));
    piVar13 = piVar13 + 0x4;  // T* element step
    iVar11 = uVar15 + (uVar8 == 0);
    uVar15 = uVar15 + 1;
    if (iVar7 == 0x7f) {
      (*(uint32_t*)rtguest::G((uint32_t)apuStack_c0 + (uint32_t)(0) * 4)) = puVar2;
    }
    else {
      ::sprintf((char *)rtguest::G((uint32_t)(apuStack_c0)), (char *)rtguest::G((uint32_t)(0x334510)), iVar7);
    }
    iVar7 = RT_CALLRA(0x252f4c, guest::uiTextWidth(apuStack_c0));
    RT_CALLRA(0x252f60, guest::DrawTextHalfY(0x10e - iVar7,iVar11 * 0x18 + 0x88,apuStack_c0,0));
    uVar4 = (*(uint32_t*)rtguest::G(0x334518));
    puVar3 = (*(uint32_t*)rtguest::G(0x00334508));
    uVar8 = uVar15 ^ 9;
  } while ((int)uVar15 < 10);
  piVar16 = (uint32_t)(iVar9 + 0x8084);
  iVar7 = 0;
  piVar13 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) + 0x80a0);
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)piVar13));
  do {
    piVar13 = piVar13 + 0x4;  // T* element step
    if ((iVar9 == 0) || (uVar14 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 8))) & 0x20) == 0)) {
      uVar14 = 0;
    }
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)piVar16));
    piVar16 = piVar16 + 0x4;  // T* element step
    iVar11 = iVar7 + (uint32_t)(iVar7 == 9);
    iVar7 = iVar7 + 1;
    iVar11 = iVar11 * 0x18 + 0x88;
    if (iVar9 == 0x7f) {
      (*(uint32_t*)rtguest::G((uint32_t)apuStack_a0 + (uint32_t)(0) * 4)) = puVar3;
LAB_0025303c:
      iVar9 = RT_CALLRA(0x25300c, guest::uiTextWidth(apuStack_a0));
      RT_CALLRA(0x253020, guest::DrawTextHalfY(0x186 - iVar9,iVar11,apuStack_a0,uVar14));
    }
    else {
      if (iVar9 != 0) {
        ::sprintf((char *)rtguest::G((uint32_t)(apuStack_a0)), (char *)rtguest::G((uint32_t)(0x334520)));
        goto LAB_0025303c;
      }
      (*(uint16_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 2)) = uVar4;
      iVar9 = RT_CALLRA(0x25303c, guest::uiTextWidth(auStack_90));
      RT_CALLRA(0x253050, guest::DrawTextHalfY(0x186 - iVar9,iVar11,auStack_90,uVar14));
    }
    uVar5 = (*(uint32_t*)rtguest::G(0x334518));
    puVar2 = (*(uint32_t*)rtguest::G(0x00334508));
    if (9 < iVar7) break;
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)piVar13));
  } while( true );
  iVar7 = 0;
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) + 0x80a0)));
  piVar13 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_5c)) + 0x8050);
  do {
    uVar14 = 0;
    if ((iVar9 != 0) && (uVar14 = 6, ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 8))) & 0x20) == 0)) {
      uVar14 = 0;
    }
    iVar9 = iVar7 + (uint32_t)(iVar7 == 9);
    iVar7 = iVar7 + 1;
    iVar9 = iVar9 * 0x18 + 0x88;
    if ((*(int32_t*)rtguest::G((uint32_t)piVar13)) == 0x7f) {
      (*(uint32_t*)rtguest::G((uint32_t)apuStack_80 + (uint32_t)(0) * 4)) = puVar2;
LAB_00253120:
      iVar11 = RT_CALLRA(0x2530f4, guest::uiTextWidth(apuStack_80));
      RT_CALLRA(0x253108, guest::DrawTextHalfY(500 - iVar11,iVar9,apuStack_80,uVar14));
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)piVar13)) != 0) {
        ::sprintf((char *)rtguest::G((uint32_t)(apuStack_80)), (char *)rtguest::G((uint32_t)(0x334520)));
        goto LAB_00253120;
      }
      (*(uint16_t*)rtguest::G((uint32_t)auStack_70 + (uint32_t)(0) * 2)) = uVar5;
      iVar11 = RT_CALLRA(0x253120, guest::uiTextWidth(auStack_70));
      RT_CALLRA(0x253134, guest::DrawTextHalfY(500 - iVar11,iVar9,auStack_70,uVar14));
    }
    if (9 < iVar7) {
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x25)));
      if (bVar1 != 1) {
        if (1 < bVar1) {
          if (bVar1 != 2) {
            return 0;
          }
          sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) + 1;
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) = sVar6;
          if (sVar6 < 0x11) {
            return 0;
          }
          iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x18)));
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 2))) = 0;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x20)));
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
          RT_CALLRA(0x2531e0, guest::gs_tasks_kill(param_1));
          return 0;
        }
        if (bVar1 != 0) {
          return 0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x25))) = 1;
      }
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x30))) & 0x50) != 0) {
        RT_CALLRA(0x253198, guest::PlayUiSound(0x1a));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x20))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar17 + 0x25))) = 2;
      }
      return 0;
    }
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)piVar13 + (uint32_t)(0x15) * sizeof(int32_t)));
    piVar13 = piVar13 + 0x4;  // T* element step
  } while( true );
}


RT_GUEST_FN(0x00252cb0, guest::DrawGolfScorecard)

}  // namespace guest
