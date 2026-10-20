// guest:0x0020e140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitFlagsIfNotLocked(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 0x100000) == 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) < 0x40) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
      uiFade(0x6000000);
    }
    uiDrawTextCell(0xc,10,0x2ea6b0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
  }
  return 0;
}


RT_GUEST_FN(0x0020e140, guest::InitFlagsIfNotLocked)

}  // namespace guest
