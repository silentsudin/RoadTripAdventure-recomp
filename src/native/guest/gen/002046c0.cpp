// guest:0x002046c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawNumberFixed(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  int iVar1;
uint32_t puVar2;
  undefined4 uVar3;
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar1 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar2 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar1 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = param_4 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  param_5 = param_5 + -1;
  if (param_5 != -1) {
    do {
      iVar1 = param_3 % 10;
      param_1 = param_1 + -0x10;
      if (iVar1 < 4) {
        iVar1 = iVar1 * 0x100000 + 0x1b80000 >> 0x10;
        if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
          uVar3 = 200;
LAB_00204804:
          uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,param_1 * 0x10000 >> 0x10,
                        (param_2 + -2) * 0x10000 >> 0x10,iVar1,uVar3,0x10,0x14);
          (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
        }
      }
      else {
        iVar1 = iVar1 * 0x10 + -0x40;
        if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
          uVar3 = 0xdc;
          goto LAB_00204804;
        }
      }
      param_3 = param_3 / 10;
      param_5 = param_5 + -1;
    } while (param_5 != -1);
  }
  return param_1;
}


RT_GUEST_FN(0x002046c0, guest::uiDrawNumberFixed)

}  // namespace guest
