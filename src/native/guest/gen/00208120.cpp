// guest:0x00208120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t frameBlend(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  param_2 = param_2 * 0x90;
  if ((*(uint32_t*)rtguest::G(0x333288)) != 0) {
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0029aa60 + param_2))) = (long)(param_1 << 0xb) | 0xa24120000;
    vifDirect(0x29aa50 + param_2,9);
    return 0;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x29a940))) = (long)(param_1 * 0x8c0) | 0xa28128000;
  vifDirect(0x29a930 + param_2,9);
  return 0;
}


RT_GUEST_FN(0x00208120, guest::frameBlend)

}  // namespace guest
