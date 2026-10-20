// guest:0x002781d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PowerOffCB(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G(0x2e6f24)) = 1;
  DIntr();
  sceSifAddCmdHandler(0xffffffff80000012,0x278190,0);
  EIntr();
  (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
  (*(int32_t*)rtguest::G(0x2e6f3c)) = 1;
  return 1;
}


RT_GUEST_FN(0x002781d0, guest::PowerOffCB)

}  // namespace guest
