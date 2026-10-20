// guest:0x00229f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectProgressSlot(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  SetFlaggedSlotBit(0,param_1,0);
  (*(uint32_t*)rtguest::G((uint32_t)0x177f760 + 0)) = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed724u + (uint32_t)((int)param_1 * 0x11) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)0x17804d0 + (uint32_t)((int)param_1) * 4)) = (uint32_t)(*(uint64_t*)rtguest::G(0x177f760));
  (*(uint8_t*)rtguest::G(0x177f76c)) = (char)param_1;
  (*(uint8_t*)rtguest::G((uint32_t)0x177f760 + 4)) = (char)param_1;
  return 0;
}


RT_GUEST_FN(0x00229f70, guest::SelectProgressSlot)

}  // namespace guest
