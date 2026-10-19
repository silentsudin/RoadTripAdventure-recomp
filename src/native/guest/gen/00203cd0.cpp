// guest:0x00203cd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawArrow(uint32_t param_1, uint32_t param_2, ...) {
uint32_t puVar1;
  int iVar2;
  
  iVar2 = (*(uint32_t*)rtguest::G(0x335684));
  if ((*(uint32_t*)rtguest::G(0x335688)) != 0xd) {
    (*(uint32_t*)rtguest::G(0x335688)) = 0xd;
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      (*(uint32_t*)rtguest::G(0x335684)) = iVar2;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0xd13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  if (iVar2 < 0x9fc) {
    uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar2 * 0x10 + 0x70,param_1,(param_2 + -2) * 0x10000 >> 0x10,0x150,
                  0xe0,0x26,0x1c);
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x00203cd0, guest::uiDrawArrow)

}  // namespace guest
