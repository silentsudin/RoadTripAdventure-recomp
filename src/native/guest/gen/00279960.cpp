// guest:0x00279960 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _lmcGetClientPtr(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = 0x01776100;
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = 0x002e89b8;
  (*(uint32_t*)rtguest::G(0x177613c)) = (*(uint32_t*)rtguest::G(0x2e89bc));
  return 0x01774b40;
}


RT_GUEST_FN(0x00279960, guest::_lmcGetClientPtr)

}  // namespace guest
