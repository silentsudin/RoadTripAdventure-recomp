// guest:0x0020df98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateInputDeviceState(uint32_t param_1, ...) {
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) = (*(uint8_t*)rtguest::G(0x20e1b8));
  }
  if ((*(uint32_t*)rtguest::G(0x333374)) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) = (*(uint32_t*)rtguest::G(0x333374));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = (*(uint8_t*)rtguest::G(0x20d120));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
    (*(uint32_t*)rtguest::G(0x333374)) = 0;
    loadStep();
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
  return 0;
}


RT_GUEST_FN(0x0020df98, guest::UpdateInputDeviceState)

}  // namespace guest
