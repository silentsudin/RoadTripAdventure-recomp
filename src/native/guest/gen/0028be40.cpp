// guest:0x0028be40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifFreeSysMemory(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  RT_CALLRA(0x28be48, guest::sceSifFreeIopHeap(q0));  // MIPS forwards its own $a0 into the free path (Ghidra dropped it)
  return 0;
}


RT_GUEST_FN(0x0028be40, guest::sceSifFreeSysMemory)

}  // namespace guest
