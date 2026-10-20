// guest:0x00286f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifCmd1StoreIndexed(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) * 4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c)))))) =
       (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
  return 0;
}


RT_GUEST_FN(0x00286f20, guest::sifCmd1StoreIndexed)

}  // namespace guest
