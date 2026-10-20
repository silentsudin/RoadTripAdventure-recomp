// guest:0x00200a60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawPadGlyph(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
uint32_t puVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  
  if (((*(uint32_t*)rtguest::G(0x335688)) != 1) && ((*(uint32_t*)rtguest::G(0x335688)) = 1, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
    iVar1 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    puVar2 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar1 + 0x70);
    (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x113780001400000;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
  }
  if (param_3 == 8) {
    if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
      return 0;
    }
    iVar3 = ((int)param_2 << 0x11) >> 0x10;
    iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
    uVar4 = 0x88;
    uVar5 = 0xe0;
    uVar6 = 0x18;
    uVar7 = 0xc;
  }
  else if (param_3 < 9) {
    if (param_3 == 2) {
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0x88;
      uVar5 = 0xec;
      uVar6 = 0x18;
      uVar7 = 0xc;
    }
    else if (param_3 < 3) {
      if (param_3 != 1) {
        return 0;
      }
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0x70;
      uVar5 = 0xec;
      uVar6 = 0x18;
      uVar7 = 0xc;
    }
    else {
      if (param_3 != 4) {
        return 0;
      }
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0x70;
      uVar5 = 0xe0;
      uVar6 = 0x18;
      uVar7 = 0xc;
    }
  }
  else {
    if (param_3 == 0x20) {
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 200;
    }
    else if (param_3 < 0x21) {
      if (param_3 != 0x10) {
        return 0;
      }
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0xb4;
    }
    else if (param_3 == 0x40) {
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0xdc;
    }
    else {
      if (param_3 != 0x80) {
        return 0;
      }
      if (0x9fb < (*(uint32_t*)rtguest::G(0x335684))) {
        return 0;
      }
      iVar3 = ((int)param_2 << 0x11) >> 0x10;
      iVar1 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70;
      uVar4 = 0xa0;
    }
    uVar5 = 0xe0;
    uVar6 = 0x14;
    uVar7 = 0x14;
  }
  RT_CALLRA(0x200da0, guest::uiSpriteWrite(iVar1,param_1,iVar3,uVar4,uVar5,uVar6,uVar7));
  (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  return 0;
}


RT_GUEST_FN(0x00200a60, guest::uiDrawPadGlyph)

}  // namespace guest
