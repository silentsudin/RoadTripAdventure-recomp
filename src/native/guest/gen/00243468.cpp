// guest:0x00243468 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t DispatchByIndex16(uint64_t param_1, ...) {
  undefined8 uVar1;
  
  if (param_1 < 0x10) {
                    /* WARNING: Could not recover jumptable at 0x00243484. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2f1240u + (uint32_t)((int)param_1) * 4))))));
    return uVar1;
  }
  return 0;
}


RT_GUEST_FN(0x00243468, guest::DispatchByIndex16)

}  // namespace guest
