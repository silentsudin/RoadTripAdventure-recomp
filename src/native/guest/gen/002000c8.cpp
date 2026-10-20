// guest:0x002000c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gsInitDisplay(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  if (param_1 == 0) {
    RT_CALLRA(0x2000e0, guest::sceGsResetGraph(0,1,2,1));
    RT_CALLRA(0x200104, guest::sceGsSetDefDBuffDc(0x335c80,0,0x280,0xe0,2,0x31,1));
    RT_CALLRA(0x200128, guest::sceGsSetDefDBuffDc(0x335fb0,0,0x280,0xe0,2,0x31,1));
  }
  else {
    RT_CALLRA(0x200140, guest::sceGsResetGraph(0,1,2,1));
    RT_CALLRA(0x200164, guest::sceGsSetDefDBuffDc(0x335c80,0,0x200,0x100,2,0x31,1));
    RT_CALLRA(0x200188, guest::sceGsSetDefDBuffDc(0x335fb0,0,0x200,0x100,2,0x31,1));
  }
  (*(uint64_t*)rtguest::G(0x336110)) = 0x32001;
  (*(uint64_t*)rtguest::G(0x336280)) = 0x32001;
  return 0;
}


RT_GUEST_FN(0x002000c8, guest::gsInitDisplay)

}  // namespace guest
