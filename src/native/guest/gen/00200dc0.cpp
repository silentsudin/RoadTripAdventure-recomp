// guest:0x00200dc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawIcon3(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t puVar1;
  int iVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  
  if (param_3 == 1) {
    iVar2 = (*(uint32_t*)rtguest::G(0x335684));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 2) && ((*(uint32_t*)rtguest::G(0x335688)) = 2, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = iVar2;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0x213780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    if (0x9fb < iVar2) {
      return 0;
    }
    uVar4 = 0x118;
    uVar5 = 0x54;
    uVar3 = 0x100;
    iVar2 = (*(uint32_t*)rtguest::G(0x335680)) + iVar2 * 0x10 + 0x70;
    uVar6 = 0x20;
  }
  else if (param_3 < 2) {
    if (param_3 != 0) {
    }
    iVar2 = (*(uint32_t*)rtguest::G(0x335684));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 2) && ((*(uint32_t*)rtguest::G(0x335688)) = 2, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = iVar2;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0x213780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    if (0x9fb < iVar2) {
    }
    uVar4 = 0x118;
    uVar5 = 0x54;
    uVar3 = 0x154;
    iVar2 = (*(uint32_t*)rtguest::G(0x335680)) + iVar2 * 0x10 + 0x70;
    uVar6 = 0x20;
  }
  else {
    if (param_3 != 2) {
    }
    iVar2 = (*(uint32_t*)rtguest::G(0x335684));
    if (((*(uint32_t*)rtguest::G(0x335688)) != 5) && ((*(uint32_t*)rtguest::G(0x335688)) = 5, (*(uint32_t*)rtguest::G(0x335684)) < 0xa00)) {
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = iVar2;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0x513780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
    if (0x9fb < iVar2) {
    }
    uVar4 = 0xdc;
    uVar5 = 0x22;
    uVar3 = 0x1d8;
    iVar2 = (*(uint32_t*)rtguest::G(0x335680)) + iVar2 * 0x10 + 0x70;
    uVar6 = 0x1e;
  }
  uiSpriteWrite(iVar2,param_1,param_2,uVar3,uVar4,uVar5,uVar6);
  (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
}


RT_GUEST_FN(0x00200dc0, guest::uiDrawIcon3)

}  // namespace guest
