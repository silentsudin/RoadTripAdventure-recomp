// guest:0x0021cbb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t KillHookClear198(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x198))) = 0;
  return 0;
}


RT_GUEST_FN(0x0021cbb8, guest::KillHookClear198)

}  // namespace guest
