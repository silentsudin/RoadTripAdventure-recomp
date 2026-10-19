// guest:0x00203990 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiSetTexa(uint64_t param_1, ...) {
uint32_t puVar1;
  
  if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
    puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70);
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = param_1 | 0x8000 | (long)((int)param_1 / 2) << 0x20;
    (*(uint64_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x3b;
  }
  return 0;
}


RT_GUEST_FN(0x00203990, guest::uiSetTexa)

}  // namespace guest
