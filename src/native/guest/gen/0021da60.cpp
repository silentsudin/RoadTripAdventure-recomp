// guest:0x0021da60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildDmaPacketSlot4C(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  ulong uVar1;
  ulong uVar2;
  
  uVar1 = (*(uint64_t*)rtguest::G(0x29cc30));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x29cc30));
  uVar2 = (*(uint64_t*)rtguest::G(0x29cc38));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = uVar1 & 0xffffffffffff8000 | 4;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar2;
  RT_CALLRA(0x21daa0, guest::vu0TransformVerts(param_1 + 2 * sizeof(uint64_t),param_2,4,1));
  return 1;
}


RT_GUEST_FN(0x0021da60, guest::BuildDmaPacketSlot4C)

}  // namespace guest
