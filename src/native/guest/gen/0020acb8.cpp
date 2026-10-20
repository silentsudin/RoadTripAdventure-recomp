// guest:0x0020acb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_controls_rumble_start(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  
  if ((((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + param_2 + 0xc))) & (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xe)))) != 0) && (param_4 != 0)) {
    puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(0x29ad60u + (uint32_t)(param_3) * 4)));
    if (0xff < param_4) {
      param_4 = 0xff;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00335698 + param_2))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)0x3356a0 + param_2)) = (char)param_4;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x00333358 + param_2 * 4))) = puVar1;
  }
  return 0;
}


RT_GUEST_FN(0x0020acb8, guest::gs_controls_rumble_start)

}  // namespace guest
