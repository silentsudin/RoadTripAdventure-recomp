// guest:0x00286690 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sioPutCharCRLF(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  if (param_1 == 10) {
    sioPutChar(0xd);
    sioPutChar(10);
    return 0;
  }
  sioPutChar(param_1);  // MIPS tail-calls with $4 = param_1 passed through (Ghidra dropped it)
  return 0;
}


RT_GUEST_FN(0x00286690, guest::sioPutCharCRLF)

}  // namespace guest
