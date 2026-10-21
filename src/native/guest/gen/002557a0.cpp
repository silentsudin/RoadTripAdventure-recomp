// guest:0x002557a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawPaintCursorSprites(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  uint uVar2;
  
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) != 0) {
    iVar1 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) - ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) >> 0x1f) & 0xeU) * 2;
    uiSpriteWrite(param_2 + 0x3220,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x2ac600))),
                  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2ac602u + iVar1))),0,0,0x20,0x20);
    uVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) / 2 + 4U & 7;
    uiSpriteWrite(param_2 + 0x3270,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(uVar2 * 4 + 0x2ac600))),
                  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x2ac602u + uVar2))),0,0,0x20,0x20);
    return 0;
  }
  uiSpriteWrite(param_2 + 0x3220,(*(uint16_t*)rtguest::G(0x2ac620)),(*(uint32_t*)rtguest::G(0x2ac622)),0,0,0x20,0x20);
  uiSpriteWrite(param_2 + 0x3270,(*(uint16_t*)rtguest::G(0x2ac620)),(*(uint32_t*)rtguest::G(0x2ac622)),0,0,0x20,0x20);
  return 0;
}


RT_GUEST_FN(0x002557a0, guest::DrawPaintCursorSprites)

}  // namespace guest
