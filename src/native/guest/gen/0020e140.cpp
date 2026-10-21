// guest:0x0020e140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitFlagsIfNotLocked(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 0x100000) == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) < 0x40) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
      RT_CALLRA(0x20e180, guest::uiFade(0x6000000));
    }
    RT_CALLRA(0x20e198, guest::uiDrawTextCell(0xc,10,0x2ea6b0,0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
  }
  return 0;
}


RT_GUEST_FN(0x0020e140, guest::InitFlagsIfNotLocked)

}  // namespace guest
