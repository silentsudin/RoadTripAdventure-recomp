// guest:0x0027c8e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __register_frame(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
uint32_t pvVar1;
  
  pvVar1 = RT_CALLRA(0x27c8f0, guest::malloc(0x18));
  __register_frame_info(param_1,pvVar1);
  return 0;
}


RT_GUEST_FN(0x0027c8e0, guest::__register_frame)

}  // namespace guest
