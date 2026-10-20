// guest:0x0020df98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateInputDeviceState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    // pseudocode stores the code pointer AbortCdOperation (guest fn @0x20e1b8)
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) = 0x0020e1b8;
  }
  if ((*(uint32_t*)rtguest::G(0x333374)) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) = (*(uint32_t*)rtguest::G(0x333374));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0;
    // pseudocode stores the code pointer loadStep (guest fn @0x20d120)
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x0020d120;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
    (*(uint32_t*)rtguest::G(0x333374)) = 0;
    // MIPS 0x20dffc: j 0x20d120 (loadStep) — tail jump with $a0 = $v1 = node
    // (set 0x20df9c), $a1 = ctx untouched. The transliteration dropped both
    // args (host-register garbage picked a random dispatch case and staged
    // EFFECT.GSL over the car-model buffer — CAR SIZE OVER at 0x20c7b8).
    loadStep(param_1,param_2);
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
  return 0;
}


RT_GUEST_FN(0x0020df98, guest::UpdateInputDeviceState)

}  // namespace guest
