// guest:0x00270bb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDmaPause(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0xfffffeff;
  REG_DMAC_ENABLEW = 0;
  return 0;
}


RT_GUEST_FN(0x00270bb8, guest::sceDmaPause)

}  // namespace guest
