// guest:0x002080d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectMeshIntersectionRoutine(uint32_t param_1, ...) {
  if (param_1 == 0) {
    (*(uint32_t*)rtguest::G(0x335710)) = (*(uint8_t*)rtguest::G(0x207280));
  }
  else {
    (*(uint32_t*)rtguest::G(0x335710)) = (*(uint8_t*)rtguest::G(0x2084d0));
  }
  return 0;
}


RT_GUEST_FN(0x002080d8, guest::SelectMeshIntersectionRoutine)

}  // namespace guest
