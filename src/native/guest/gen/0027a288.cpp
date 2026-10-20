// guest:0x0027a288 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mceGetInfoApdx(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  
  puVar1 = (uint32_t)(param_1 | 0x20000000);
  if ((*(uint32_t*)rtguest::G(0x1774b68)) != (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1774b68)))) = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
  }
  if ((*(uint32_t*)rtguest::G(0x1774b6c)) != (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1774b6c)))) = (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t)));
  }
  if ((*(uint32_t*)rtguest::G(0x1774b70)) != (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1774b70)))) = (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x24) * sizeof(uint32_t)));
  }
  return 0;
}


RT_GUEST_FN(0x0027a288, guest::mceGetInfoApdx)

}  // namespace guest
