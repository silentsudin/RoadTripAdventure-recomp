// guest:0x00235570 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMaxCounter846C(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  if ((*(uint32_t*)rtguest::G(0x1780a2c)) < (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c)))) {
    (*(uint32_t*)rtguest::G(0x1780a2c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x846c)));
    return 1;
  }
  return 0;
}


RT_GUEST_FN(0x00235570, guest::UpdateMaxCounter846C)

}  // namespace guest
