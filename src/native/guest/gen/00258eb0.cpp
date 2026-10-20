// guest:0x00258eb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BgmStop1(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  MusicRpcCmd15(1);
  return 0;
}


RT_GUEST_FN(0x00258eb0, guest::BgmStop1)

}  // namespace guest
