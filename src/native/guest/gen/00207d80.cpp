// guest:0x00207d80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gifChainEndTag(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(272);
  (*(uint16_t*)rtguest::G((uint32_t)param_1)) = param_2;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 3))) = 0x70;
  (*(uint32_t*)rtguest::G((uint32_t)(param_1 + 2 * sizeof(uint16_t)))) = 0;
  return 0;
}


RT_GUEST_FN(0x00207d80, guest::gifChainEndTag)

}  // namespace guest
