// guest:0x00223200 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetShowroomLighting(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  RT_CALLRA(0x223210, guest::vifDirect(0x29d240,5));
  RT_CALLRA(0x22321c, guest::vifUploadLightAt16(0x177f5e0));
  (*(uint32_t*)rtguest::G(0x3356e8)) = 0;
  return 0;
}


RT_GUEST_FN(0x00223200, guest::ResetShowroomLighting)

}  // namespace guest
