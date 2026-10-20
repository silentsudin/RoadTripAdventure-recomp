// guest:0x0028d008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _InitSys(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  RT_CALLRA(0x28d010, guest::supplement_crt0());
  RT_CALLRA(0x28d018, guest::InitAlarm());
  InitThread();
  return 0;
}


RT_GUEST_FN(0x0028d008, guest::_InitSys)

}  // namespace guest
