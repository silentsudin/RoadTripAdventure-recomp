// guest:0x00234f38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetTrackMeta2(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8428))) = param_2;
  return 0;
}


RT_GUEST_FN(0x00234f38, guest::SetTrackMeta2)

}  // namespace guest
