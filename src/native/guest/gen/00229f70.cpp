// guest:0x00229f70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectProgressSlot(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  RT_CALLRA(0x229f88, guest::SetFlaggedSlotBit(0,param_1,0));
  (*(uint32_t*)rtguest::G((uint32_t)0x177f760 + 0)) = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed724u + (uint32_t)((int)param_1 * 0x11) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)0x17804d0 + (uint32_t)((int)param_1) * 4)) = (uint32_t)(*(uint64_t*)rtguest::G(0x177f760));
  (*(uint8_t*)rtguest::G(0x177f76c)) = (char)param_1;
  (*(uint8_t*)rtguest::G((uint32_t)0x177f760 + 4)) = (char)param_1;
  return 0;
}


RT_GUEST_FN(0x00229f70, guest::SelectProgressSlot)

}  // namespace guest
