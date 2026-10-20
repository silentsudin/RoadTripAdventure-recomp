// guest:0x00270180 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaGetChan(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if (param_1 < 10) {
    return (*(uint32_t*)rtguest::G((uint32_t)(0x2dcdc0u + (uint32_t)((int)param_1) * 4)));
  }
  return (uint32_t)0x0;
}


RT_GUEST_FN(0x00270180, guest::sceDmaGetChan)

}  // namespace guest
