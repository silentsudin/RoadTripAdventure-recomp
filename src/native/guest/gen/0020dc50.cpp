// guest:0x0020dc50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadRequestTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  if ((*(uint32_t*)rtguest::G(0x333370)) == 0) {
    (*(uint32_t*)rtguest::G(0x333370)) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) = 1;
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0;
    (*(uint32_t*)rtguest::G(0x333374)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (((*(uint32_t*)rtguest::G(0x333374)) & 0x100000) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    }
    gs_tasks_kill(param_1); // arg-drop fix: j 0x204de8 at 0x20dc9c, $a0=node unmodified (capstone)
    return 0;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 0x100000) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
  }
  return 0;
}


RT_GUEST_FN(0x0020dc50, guest::loadRequestTask)

}  // namespace guest
