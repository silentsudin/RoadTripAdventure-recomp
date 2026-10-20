// guest:0x0020cdc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t DispatchInputMapping(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  undefined8 uVar1;
  
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) == '\0') && ((*(uint32_t*)rtguest::G(0x3356c0)) == 0)) {
    (*(uint32_t*)rtguest::G(0x3356c0)) = 5;
  }
  if (7 < (*(uint32_t*)rtguest::G(0x3356c0))) {
    return 0;
  }
                    /* WARNING: Could not recover jumptable at 0x0020ce1c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  uVar1 = rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x2ea690u + (uint32_t)((*(uint32_t*)rtguest::G(0x3356c0))) * 4))))));
  return uVar1;
}


RT_GUEST_FN(0x0020cdc0, guest::DispatchInputMapping)

}  // namespace guest
