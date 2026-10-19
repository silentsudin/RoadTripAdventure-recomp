// guest:0x00279c40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcMkdir(uint32_t q0, ...) {
  long lVar1;
  
  lVar1 = sceMcOpen();
  if (lVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x2e89b8)) = 0xb;
  }
  return lVar1;
}


RT_GUEST_FN(0x00279c40, guest::sceMcMkdir)

}  // namespace guest
