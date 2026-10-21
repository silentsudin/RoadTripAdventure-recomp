// guest:0x00255070 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildPaintEditorMenuSprites(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x38, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::gS[1] = 0x000000000000004eull;  // walk const
    rtguest::gS[2] = 0x0000000000000001ull;  // walk const
    rtguest::gS[3] = 0x0000000000000028ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t puVar1;
  int iVar2;
  int iVar3;
  short sVar4;
  
  RT_CALLRA(0x2550b8, guest::gifBeginSpriteBatch(param_1,0x20045dc669329f2e));
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x50))) = 0x80808080;
  puVar1 = (uint32_t)(param_1 + 0xc0);
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0x5380b;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x58))) = 1;
  gifSetSpriteCount(param_1 + 0x60,1);
  gifSetSpriteCount(puVar1,0x82);
  (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = (*(uint64_t*)rtguest::G((uint32_t)puVar1)) & 0xffffffffffff8000 | (long)(int)(((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar1)) & 0x7fff) + 0x67 & 0x7fff);
  RT_CALLRA(0x255140, guest::uiSpriteWrite(param_1 + 0xd0,0x24,0x34,0x50,0x132,0x30,0x28));
  RT_CALLRA(0x255168, guest::uiSpriteWriteWrap(param_1 + 0x120,0x54,0x34,0x80,0x132,0xdc,0x28,8));
  RT_CALLRA(0x255188, guest::uiSpriteWrite(param_1 + 0x170,0x130,0x34,0x88,0x132,0x12,0x28));
  RT_CALLRA(0x2551a8, guest::uiSpriteWrite(param_1 + 0x1c0,0x24,0x1e,0,0x116,100,0x1a));
  RT_CALLRA(0x2551c8, guest::uiSpriteWrite(param_1 + 0x210,0x2a,0x21,0x80,0xf0,0x5c,0x14));
  RT_CALLRA(0x2551e8, guest::uiSpriteWrite(param_1 + 0x260,0x24,0x76,0,0x132,0x30,0x4e));
  RT_CALLRA(0x255210, guest::uiSpriteWriteWrap(param_1 + 0x2b0,0x54,0x76,0x30,0x132,0xdc,0x4e,8));
  RT_CALLRA(0x255230, guest::uiSpriteWrite(param_1 + 0x300,0x130,0x76,0x38,0x132,0x12,0x4e));
  RT_CALLRA(0x255250, guest::uiSpriteWrite(param_1 + 0x350,0x24,0x60,0,0x116,100,0x1a));
  RT_CALLRA(0x255270, guest::uiSpriteWrite(param_1 + 0x3a0,0x2a,99,0x80,0x104,0x5c,0x14));
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8a8))) = 1;
  RT_CALLRA(0x255298, guest::uiSpriteWrite(param_1 + 0x8b0,0x24,0xc4,0,0x132,0x30,0x4e));
  RT_CALLRA(0x2552c0, guest::uiSpriteWriteWrap(param_1 + 0x900,0x54,0xc4,0x30,0x132,0xdc,0x4e,8));
  RT_CALLRA(0x2552e0, guest::uiSpriteWrite(param_1 + 0x950,0x130,0xc4,0x38,0x132,0x12,0x4e));
  RT_CALLRA(0x255300, guest::uiSpriteWrite(param_1 + 0x3f0,0x24,300,0,0x132,0x30,0x4e));
  RT_CALLRA(0x255328, guest::uiSpriteWriteWrap(param_1 + 0x440,0x54,300,0x30,0x132,0xdc,0x4e,8));
  RT_CALLRA(0x255348, guest::uiSpriteWrite(param_1 + 0x490,0x130,300,0x38,0x132,0x12,0x4e));
  RT_CALLRA(0x255368, guest::uiSpriteWrite(param_1 + 0x4e0,0x24,0x116,0,0x116,100,0x1a));
  RT_CALLRA(0x255388, guest::uiSpriteWrite(param_1 + 0x530,0x2a,0x119,0x80,0x118,0x5c,0x14));
  RT_CALLRA(0x2553a8, guest::uiSpriteWrite(param_1 + 0x580,0x66,0x13e,0x118,0xe0,0xe4,0x2a));
  RT_CALLRA(0x2553c8, guest::uiSpriteWrite(param_1 + 0x5d0,0x24,0x180,0x50,0x132,0x30,0x28));
  RT_CALLRA(0x2553f0, guest::uiSpriteWriteWrap(param_1 + 0x620,0x54,0x180,0x80,0x132,0xdc,0x28,8));
  RT_CALLRA(0x255410, guest::uiSpriteWrite(param_1 + 0x670,0x130,0x180,0x88,0x132,0x12,0x28));
  RT_CALLRA(0x255430, guest::uiSpriteWrite(param_1 + 0x9b0,0x5b,0x83,0x100,0xf0,0x10,0x10));
  RT_CALLRA(0x255450, guest::uiSpriteWrite(param_1 + 0xa10,0x5b,0x96,0x100,0x100,0x10,0x10));
  RT_CALLRA(0x255470, guest::uiSpriteWrite(param_1 + 0xa70,0x5b,0xa9,0x100,0x110,0x10,0x10));
  RT_CALLRA(0x255490, guest::uiSpriteWrite(param_1 + 0xad0,0x5b,0xd1,0x100,0xf0,0x10,0x10));
  RT_CALLRA(0x2554b0, guest::uiSpriteWrite(param_1 + 0xb30,0x5b,0xe4,0x100,0x100,0x10,0x10));
  RT_CALLRA(0x2554d0, guest::uiSpriteWrite(param_1 + 0xb90,0x5b,0xf7,0x100,0x110,0x10,0x10));
  iVar2 = 0xf;
  sVar4 = 0x70;
  iVar3 = param_1;
  do {
    RT_CALLRA(0x255504, guest::uiSpriteWrite(iVar3 + 0xbf0,sVar4,0x85,0x80,0xe0,10,10));
    RT_CALLRA(0x255524, guest::uiSpriteWrite(iVar3 + 0x11f0,sVar4,0x98,0x80,0xe0,10,10));
    RT_CALLRA(0x255544, guest::uiSpriteWrite(iVar3 + 0x17f0,sVar4,0xab,0x80,0xe0,10,10));
    RT_CALLRA(0x255564, guest::uiSpriteWrite(iVar3 + 0x1df0,sVar4,0xd3,0x80,0xe0,10,10));
    RT_CALLRA(0x255584, guest::uiSpriteWrite(iVar3 + 0x23f0,sVar4,0xe6,0x80,0xe0,10,10));
    iVar2 = iVar2 + -1;
    RT_CALLRA(0x2555a8, guest::uiSpriteWrite(iVar3 + 0x29f0,sVar4,0xf9,0x80,0xe0,10,10));
    iVar3 = iVar3 + 0x60;
    sVar4 = sVar4 + 10;
  } while (-1 < iVar2);
  puVar1 = (uint32_t)(param_1 + 0x2fe0);
  gifSetSpriteCount(puVar1,0x60);
  (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = (*(uint64_t*)rtguest::G((uint32_t)puVar1)) & 0xfc007fffffff8000 | 0x23000000000012;
  iVar3 = param_1 + 0x3150;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30e0))) = 0x80ff0000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3050))) = 0x80ff0000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3080))) = 0x800000ff;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30b0))) = 0x8000ff00;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2ff0))) = 0x800000ff;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3020))) = 0x8000ff00;
  RT_CALLRA(0x255654, guest::gifAdPair(param_1 + 0x3110,0x48,0x42));
  RT_CALLRA(0x25567c, guest::gifSetTexRegs(param_1 + 0x3130,0x20046245dd40a2f2,0xffff0000));
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3138))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3138))) & 0xffffffffffffff0f | 0x10;
  RT_CALLRA(0x255694, guest::gifQuadTag(iVar3));
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x3160))) = 4;
  RT_CALLRA(0x2556b4, guest::uiSetQuadXY(iVar3,0,0,0x80,0x80));
  RT_CALLRA(0x2556d0, guest::uiMoveSprite(iVar3,0,400,0x10,0xfffffc));
  RT_CALLRA(0x2556ec, guest::uiMoveSprite(iVar3,1,0x236,0x10,0xfffffc));
  RT_CALLRA(0x255708, guest::uiMoveSprite(iVar3,2,0x170,0x100,0xfffffc));
  RT_CALLRA(0x255724, guest::uiMoveSprite(iVar3,3,0x256,0x100,0xfffffc));
  RT_CALLRA(0x255758, guest::gifSetTexRegs(param_1 + 0x31f0,0x200462a55540a313,0x8080808000));
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x31f8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x31f8))) & 0xffffffffffffff0f | 0x10;
  gifSetSpriteCount(param_1 + 0x3210,2);
  return 0;
}


RT_GUEST_FN(0x00255070, guest::BuildPaintEditorMenuSprites)

}  // namespace guest
