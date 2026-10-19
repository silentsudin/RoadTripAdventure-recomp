// guest:0x00284658 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _mprec_log10(uint32_t param_1, ...) {
  undefined8 uVar1;
  
  uVar1 = 0x3ff0000000000000;
  if (param_1 < 0x18) {
    uVar1 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00332430 + param_1 * 8)));
  }
  else {
    for (; 0 < param_1; param_1 = param_1 + -1) {
      uVar1 = dpmul(uVar1,0x4024000000000000);
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x00284658, guest::_mprec_log10)

}  // namespace guest
