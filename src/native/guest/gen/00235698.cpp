// guest:0x00235698 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMaxCounter847C(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((*(uint32_t*)rtguest::G(0x1780a28)) < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x847c)))) {
    (*(uint32_t*)rtguest::G(0x1780a28)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x847c)));
    return 1;
  }
  return 0;
}


RT_GUEST_FN(0x00235698, guest::UpdateMaxCounter847C)

}  // namespace guest
