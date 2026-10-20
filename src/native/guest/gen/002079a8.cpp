// guest:0x002079a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMoveSprite(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(0);
  param_2 = param_2 * 0x20;
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x30))) = param_3 * 0x10 + 0x6c00;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x34))) = param_4 * 8 + 0x7900;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x30))) = (param_3 << 6) / 5 + 0x7000;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x34))) = param_4 * 8 + 0x7800;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x38))) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + param_1 + 0x3c))) = 0;
  return 0;
}


RT_GUEST_FN(0x002079a8, guest::uiMoveSprite)

}  // namespace guest
