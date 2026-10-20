// guest:0x00201fd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawNumber16Y2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = (*(uint32_t*)rtguest::G(0x335684));
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = param_4 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  do {
    param_1 = param_1 + -0x10;
    iVar2 = param_3 % 10;
    if (iVar2 < 4) {
      iVar2 = iVar2 * 0x100000 + 0x1b80000 >> 0x10;
      if (iVar3 < 0x9fc) {
        uVar4 = 200;
LAB_00202104:
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar3 * 0x10 + 0x70,param_1 * 0x10000 >> 0x10,
                      (param_2 + -2) * 0x10000 >> 0x10,iVar2,uVar4,0x10,0x14);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
    }
    else {
      iVar2 = iVar2 * 0x10 + -0x40;
      if (iVar3 < 0x9fc) {
        uVar4 = 0xdc;
        goto LAB_00202104;
      }
    }
    param_3 = param_3 / 10;
    iVar3 = (*(uint32_t*)rtguest::G(0x335684));
    if (param_3 == 0) {
      return param_1;
    }
  } while( true );
}


RT_GUEST_FN(0x00201fd0, guest::uiDrawNumber16Y2)

}  // namespace guest
