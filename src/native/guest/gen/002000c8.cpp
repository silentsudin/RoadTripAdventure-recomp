// guest:0x002000c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gsInitDisplay(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  if (param_1 == 0) {
    sceGsResetGraph(0,1,2,1);
    sceGsSetDefDBuffDc(0x335c80,0,0x280,0xe0,2,0x31,1);
    sceGsSetDefDBuffDc(0x335fb0,0,0x280,0xe0,2,0x31,1);
  }
  else {
    sceGsResetGraph(0,1,2,1);
    sceGsSetDefDBuffDc(0x335c80,0,0x200,0x100,2,0x31,1);
    sceGsSetDefDBuffDc(0x335fb0,0,0x200,0x100,2,0x31,1);
  }
  (*(uint64_t*)rtguest::G(0x336110)) = 0x32001;
  (*(uint64_t*)rtguest::G(0x336280)) = 0x32001;
  return 0;
}


RT_GUEST_FN(0x002000c8, guest::gsInitDisplay)

}  // namespace guest
