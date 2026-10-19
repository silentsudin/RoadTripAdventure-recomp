// guest:0x0028c0e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifLoadFileReset(uint32_t q0, ...) {
  (*(uint32_t*)rtguest::G(0x2e99a0)) = 0xffffffff;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x0177a028)), 0, 4));
  return 0;
}


RT_GUEST_FN(0x0028c0e0, guest::sceSifLoadFileReset)

}  // namespace guest
