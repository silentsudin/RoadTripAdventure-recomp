// guest:0x002652b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawStampCard(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(240);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xd0, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0xa8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xb8, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xe0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0xc8, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xd8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xe8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xec) = 0x00000000u;
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
  }
    uint32_t uStack_f0 = rtguest::allocFrame(4);
    uint32_t uStack_ec = rtguest::allocFrame(4);
    uint32_t uStack_e8 = rtguest::allocFrame(4);
    uint32_t uStack_e4 = rtguest::allocFrame(4);
    uint32_t uStack_e0 = rtguest::allocFrame(4);
    uint32_t uStack_dc = rtguest::allocFrame(4);
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    uint32_t uStack_d4 = rtguest::allocFrame(4);
uint32_t acStack_d0 = rtguest::allocFrame(64);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t uStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
uint32_t auStack_70 = rtguest::allocFrame(32);
  size_t sVar1;
  long lVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  ulong uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  uVar7 = param_2 & 1;
  iVar9 = (int)param_2;
  RT_CALLRA(0x265300, guest::uiArenaSetTexture(param_1,0x2dbb50,0,0));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x210;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xffffffc0;
  if (uVar7 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0x40;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x30;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0x88;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0x38;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0x38;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0;
  RT_CALLRA(0x26534c, guest::uiArenaDataTag(param_1,1));
  RT_CALLRA(0x265368, guest::uiArenaSprites(param_1,uStack_f0,1,0,0,2));
  ::sprintf((char *)rtguest::G((uint32_t)(acStack_d0)), (char *)rtguest::G((uint32_t)(0x335308)), iVar9 + 1U);
  sVar1 = ::strlen((char *)rtguest::G((uint32_t)(acStack_d0)));
  if (uVar7 == 0) {
    iVar8 = 0x54;
  }
  else {
    iVar8 = 0x242;
  }
  iVar10 = 0;
  RT_CALLRA(0x2653bc, guest::DrawTextHalfY(iVar8 + (int)sVar1 * -10,0x44,acStack_d0,3));
  iVar8 = 0;
  RT_CALLRA(0x2653cc, guest::uiArenaDataTag(param_1,9));
  uVar3 = 0x120;
  if (uVar7 == 0) {
    uVar3 = 0;
  }
  RT_CALLRA(0x2653f0, guest::uiArenaSpritesWrap(param_1,0x2dc258,9,uVar3,0,2));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x158;
  if (uVar7 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 0x38;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 0x110;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0x60;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_84)) = 0x80;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = 0x14;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0x18;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0;
  RT_CALLRA(0x26543c, guest::uiArenaDataTag(param_1,0x30));
  do {
    iVar4 = 0xb;
    iVar6 = 0;
    do {
      iVar4 = iVar4 + -1;
      RT_CALLRA(0x265470, guest::uiArenaSprites(param_1,uStack_90,1,iVar6,iVar8 << 3,2));
      iVar6 = iVar6 + 0x14;
    } while (-1 < iVar4);
    iVar10 = iVar10 + 1;
    iVar8 = iVar8 + 3;
  } while (iVar10 < 4);
  uVar5 = iVar9 + 1U & 0xff;
  lVar2 = RT_CALLRA(0x265494, guest::TestStateBitFc78(uVar5));
  if (lVar2 != 0) {
    RT_CALLRA(0x2654a8, guest::drawStampIcon(param_1,param_2,0));
  }
  lVar2 = RT_CALLRA(0x2654b0, guest::TestStateBitFc88(uVar5));
  iVar8 = 0;
  if (lVar2 != 0) {
    while( true ) {
      lVar2 = RT_CALLRA(0x2654f0, guest::ExtractDelimitedToken(auStack_70,(*(uint32_t*)rtguest::G((uint32_t)(0x2dbba0u + (uint32_t)(iVar9) * 4))),
                         0x335300,iVar8));
      iVar10 = iVar8 * 0x18;
      if (lVar2 == 0) break;
      iVar8 = iVar8 + 1;
      RT_CALLRA(0x2654d4, guest::DrawTextHalfY((iVar9 % 2) * 0x120 + 0x3c,iVar10 + 0x110,auStack_70,3));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002652b8, guest::drawStampCard)

}  // namespace guest
