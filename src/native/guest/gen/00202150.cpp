// guest:0x00202150 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawNumber10(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
uint32_t puVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar5 = 0;
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
    if (((iVar5 == 3) || (iVar5 == 6)) || (iVar5 == 9)) {
      param_1 = param_1 + -10;
      if (iVar3 < 0x9fc) {
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar3 * 0x10 + 0x70,param_1 * 0x10000 >> 0x10,param_2,0x1ae,0xb4,
                      10,0x14);
        iVar3 = (*(uint32_t*)rtguest::G(0x335684)) + 5;
        (*(uint32_t*)rtguest::G(0x335684)) = iVar3;
      }
    }
    param_1 = param_1 + -10;
    iVar2 = param_3 % 10;
    if (iVar2 < 4) {
      iVar2 = iVar2 * 0xa0000 + 0x1d60000 >> 0x10;
      if (iVar3 < 0x9fc) {
        uVar4 = 0xb4;
LAB_002022f8:
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar3 * 0x10 + 0x70,param_1 * 0x10000 >> 0x10,param_2,iVar2,uVar4
                      ,10,0x14);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
    }
    else {
      iVar2 = iVar2 * 10 + -0x28;
      if (iVar3 < 0x9fc) {
        uVar4 = 200;
        goto LAB_002022f8;
      }
    }
    iVar5 = iVar5 + 1;
    param_3 = param_3 / 10;
    iVar3 = (*(uint32_t*)rtguest::G(0x335684));
    if (param_3 == 0) {
    }
  } while( true );
}


RT_GUEST_FN(0x00202150, guest::uiDrawNumber10)

}  // namespace guest
