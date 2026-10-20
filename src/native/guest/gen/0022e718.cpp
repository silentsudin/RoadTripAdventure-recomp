// guest:0x0022e718 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AccumulateDistanceSingle(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  if ((*(int32_t*)rtguest::G(0x177ae54)) < 0) {
    (*(int32_t*)rtguest::G(0x177ae54)) = (*(int32_t*)rtguest::G(0x177ae54)) + 0x7fff;
  }
  (*(uint32_t*)rtguest::G(0x177fdb8)) = (*(uint32_t*)rtguest::G(0x177fdb8)) + ((*(int32_t*)rtguest::G(0x177ae54)) >> 0xf);
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 8))) & 0x200) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 7)))) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 7)))) * 4)) + ((*(int32_t*)rtguest::G(0x177ae54)) >> 0xf);
  }
  (*(int32_t*)rtguest::G(0x177ae54)) = 0;
  return 0;
}


RT_GUEST_FN(0x0022e718, guest::AccumulateDistanceSingle)

}  // namespace guest
