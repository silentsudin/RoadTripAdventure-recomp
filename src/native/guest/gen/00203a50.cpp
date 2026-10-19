// guest:0x00203a50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDebugText8(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint64_t param_4, ...) {
  byte bVar1;
  uint uVar2;
  int iVar3;
uint32_t puVar4;
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      iVar3 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      puVar4 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar3 + 0x70);
      (*(uint64_t*)rtguest::G((uint32_t)puVar4)) = param_4 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  while (uVar2 = (uint32_t)bVar1, uVar2 != 0) {
    param_3 = param_3 + 1;
    if (uVar2 != 0x20) {
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        uiSpriteWriteWH((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,param_1,param_2,
                        (int)((uVar2 & 7) * 0x80000 + 0x1c00000) >> 0x10,(uVar2 & 0x78) + 0xe0,8,8);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
    }
    param_1 = param_1 + 8;
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  }
  return 0;
}


RT_GUEST_FN(0x00203a50, guest::uiDebugText8)

}  // namespace guest
