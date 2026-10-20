// guest:0x00286ec0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePrintf(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  rtguest::FrameGuard rt_fg(176);
    // spill area must be one contiguous block: _printf walks it 8 bytes per arg
    uint32_t uStack_38 = rtguest::allocFrame(0x38);
uint32_t puVar1;
  
  puVar1 = (*(uint32_t*)rtguest::G(0x002e98f8));
  // stores the guest address of scePrintfPutChar (0x2865e0) into the sink slot
  (*(uint32_t*)rtguest::G(0x002e98f8)) = 0x002865e0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = param_2;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(1) * 8)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(2) * 8)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(3) * 8)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(4) * 8)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(5) * 8)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38 + (uint32_t)(6) * 8)) = param_8;
  RT_CALLRA(0x286f00, guest::_printf(param_1,uStack_38));
  (*(uint32_t*)rtguest::G(0x002e98f8)) = puVar1;
  return 0;
}


RT_GUEST_FN(0x00286ec0, guest::scePrintf)

}  // namespace guest
