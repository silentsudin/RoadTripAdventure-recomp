// guest:0x00235880 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitScreenLayout(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  
  RT_CALLRA(0x2358ac, guest::gifBeginSpriteBatch(param_1,0x2015a70e21312cb8));
  iVar1 = (int)param_1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x50))) = 0x615a70001400000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x58))) = 0x16;
  RT_CALLRA(0x2358d8, guest::gifSetSpriteCount(iVar1 + 0x60,1));
  RT_CALLRA(0x235904, guest::uiSpriteWriteWrap(iVar1 + 0x70,0,0,0xc0,0xc0,0x140,0xe4,0x40));
  RT_CALLRA(0x235920, guest::uiSetSpriteXY(iVar1 + 0x90,iVar1 + 0xb0,0,0,0x280,0x1c1));
  RT_CALLRA(0x23592c, guest::gifSetSpriteCount(iVar1 + 0xf0,3));
  RT_CALLRA(0x235960, guest::gifSetTex0Tex2(iVar1 + 0xc0,0x201596ce21312c36,0x21596c001400000));
  RT_CALLRA(0x235980, guest::uiSpriteWrite(iVar1 + 0x100,0x8c,0x28,0,0xd0,0x40,0x30));
  RT_CALLRA(0x2359a8, guest::uiSpriteWriteWrap(iVar1 + 0x150,0xcc,0x28,0x40,0xd0,0x118,0x30,8));
  RT_CALLRA(0x2359c8, guest::uiSpriteWrite(iVar1 + 0x1a0,0x1e4,0x28,0x48,0xd0,0x10,0x30));
  return 0;
}


RT_GUEST_FN(0x00235880, guest::InitScreenLayout)

}  // namespace guest
