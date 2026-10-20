// guest:0x00203fc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t uiDrawText(uint64_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  int iVar2;
uint32_t puVar3;
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar3 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar2 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar3)) = param_4 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  while (bVar1 != 0) {
    param_1 = uiDrawGlyph(param_1,param_2 << 1,0,param_3);
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
    if (bVar1 == 0x2f) {
      param_3 = param_3 + 2;
    }
    else if (bVar1 == 0x60) {
      param_3 = param_3 + 2;
    }
    else if (bVar1 == 0x5e) {
      param_3 = param_3 + 2;
    }
    else if (bVar1 == 0x7e) {
      param_3 = param_3 + 2;
    }
    else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
      param_3 = param_3 + 2;
    }
    else {
      param_3 = param_3 + 1;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  }
  return param_1;
}


RT_GUEST_FN(0x00203fc8, guest::uiDrawText)

}  // namespace guest
