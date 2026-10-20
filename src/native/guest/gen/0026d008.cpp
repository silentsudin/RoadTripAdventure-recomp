// guest:0x0026d008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t houseVisitedMenuEntry(uint32_t q0, uint32_t q1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  HouseVisitedMenuTask(q0,q1);  // tail j 26d014, $4/$5 passthrough; callee reads both
  return 0;
}


RT_GUEST_FN(0x0026d008, guest::houseVisitedMenuEntry)

}  // namespace guest
