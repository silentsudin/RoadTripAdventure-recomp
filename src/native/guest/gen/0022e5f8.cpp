// guest:0x0022e5f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AccumulateDistanceTotals(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  if ((*(int32_t*)rtguest::G(0x177ae54)) < 0) {
    (*(int32_t*)rtguest::G(0x177ae54)) = (*(int32_t*)rtguest::G(0x177ae54)) + 0x7fff;
  }
  (*(uint32_t*)rtguest::G(0x177fdb8)) = (*(uint32_t*)rtguest::G(0x177fdb8)) + ((*(int32_t*)rtguest::G(0x177ae54)) >> 0xf);
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 8))) & 0x200) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 7)))) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177add4)) + 7)))) * 4)) + ((*(int32_t*)rtguest::G(0x177ae54)) >> 0xf);
  }
  (*(int32_t*)rtguest::G(0x177ae54)) = 0;
  if (((*(uint32_t*)rtguest::G(0x178700f)) == -0x70) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b044)) + 8))) & 0x200) != 0)) {
    iVar1 = (*(uint32_t*)rtguest::G(0x177b0c4));
    if ((*(uint32_t*)rtguest::G(0x177b0c4)) < 0) {
      iVar1 = (*(uint32_t*)rtguest::G(0x177b0c4)) + 0x7fff;
    }
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b044)) + 7)))) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b044)) + 7)))) * 4)) + (iVar1 >> 0xf);
  }
  if (((*(uint32_t*)rtguest::G(0x1787013)) == -0x60) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b2b4)) + 8))) & 0x200) != 0)) {
    iVar1 = (*(uint32_t*)rtguest::G(0x177b334));
    if ((*(uint32_t*)rtguest::G(0x177b334)) < 0) {
      iVar1 = (*(uint32_t*)rtguest::G(0x177b334)) + 0x7fff;
    }
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b2b4)) + 7)))) * 4)) =
         (*(uint32_t*)rtguest::G((uint32_t)0x177fdbc + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177b2b4)) + 7)))) * 4)) + (iVar1 >> 0xf);
  }
  return 0;
}


RT_GUEST_FN(0x0022e5f8, guest::AccumulateDistanceTotals)

}  // namespace guest
