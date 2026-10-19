// guest:0x00288ae0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceFsSemInit(uint32_t q0, ...) {
  uint32_t auStack_40 = rtguest::allocFrame(0x18);
  uint32_t uStack_3c = auStack_40 + 4;
  uint32_t uStack_38 = auStack_40 + 8;
  uint32_t uStack_2c = auStack_40 + 0x14;
  
  if ((*(int32_t*)rtguest::G(0x2e998c)) == -1) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = 1;
    (*(int32_t*)rtguest::G(0x2e998c)) = CreateSema(auStack_40);
  }
  return 0;
}


RT_GUEST_FN(0x00288ae0, guest::_sceFsSemInit)

}  // namespace guest
