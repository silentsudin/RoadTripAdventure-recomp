// guest:0x002039e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetPaletteBank(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
uint32_t puVar1;
  
  if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_1 & 0xffff)) {
    (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_1 & 0xffff);
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
      puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = param_1 << 0x38 | 0x13780001400000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002039e8, guest::uiSetPaletteBank)

}  // namespace guest
