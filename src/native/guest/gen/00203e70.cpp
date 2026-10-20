// guest:0x00203e70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawIcon20(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(16);
uint32_t puVar1;
  int iVar2;
  
  iVar2 = (*(uint32_t*)rtguest::G(0x335684));
  if ((*(uint32_t*)rtguest::G(0x335688)) != 0) {
    (*(uint32_t*)rtguest::G(0x335688)) = 0;
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      iVar2 = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      (*(uint32_t*)rtguest::G(0x335684)) = iVar2;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  if (iVar2 < 0x9fc) {
    RT_CALLRA(0x203f20, guest::uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + iVar2 * 0x10 + 0x70,param_1,param_2,
                  param_3 * 0x140000 + 0x780000 >> 0x10,0xb4,0x14,0x14));
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return 0;
}


RT_GUEST_FN(0x00203e70, guest::uiDrawIcon20)

}  // namespace guest
