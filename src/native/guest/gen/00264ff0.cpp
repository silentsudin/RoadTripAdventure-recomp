// guest:0x00264ff0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void drawBoardSpriteGrid(uint64_t param_1, uint32_t param_2) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t uStack_50 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(4);
    uint32_t iStack_44 = rtguest::allocFrame(4);
    uint32_t uStack_40 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    uint32_t uStack_34 = rtguest::allocFrame(4);
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = 0;
  RT_CALLRA(0x265018, guest::uiArenaDataTag(param_1,0x1d));
  RT_CALLRA(0x265038, guest::uiArenaSpritesWrap(param_1,0x2dbdd0,0x1d,0,0,2));
  (*(int32_t*)rtguest::G((uint32_t)iStack_44)) = param_2 * 0xc + 0x50;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = 0x38;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 0x2a;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 0x60;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) = 0x50;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0xc;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_34)) = 0;
  RT_CALLRA(0x265088, guest::uiArenaDataTag(param_1,0xc));
  uVar1 = 0;
  while( true ) {
    iVar4 = 2;
    iVar2 = (int)(uVar5 + uVar1) >> 1;
    iVar3 = iVar2 * 0x120;
    do {
      iVar4 = iVar4 + -1;
      RT_CALLRA(0x2650e8, guest::uiArenaSprites(param_1,uStack_50,1,iVar3,(uVar5 + iVar2 * -2) * 0x160,2));
      iVar3 = iVar3 + 0x50;
    } while (-1 < iVar4);
    uVar5 = uVar5 + 1;
    if (3 < (int)uVar5) break;
    uVar1 = uVar5 >> 0x1f;
  }
  return;
}


RT_GUEST_FN(0x00264ff0, guest::drawBoardSpriteGrid)

}  // namespace guest
