// guest:0x002495c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetVehicleTransmissionMode(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  
  uVar1 = 1;
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x244))) != -1) && (uVar1 = 2, (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x244))) != '\x01')) {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x002495c0, guest::GetVehicleTransmissionMode)

}  // namespace guest
