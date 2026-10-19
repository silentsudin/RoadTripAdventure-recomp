// guest:0x0026d008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t houseVisitedMenuEntry(uint32_t q0, ...) {
  HouseVisitedMenuTask();
  return 0;
}


RT_GUEST_FN(0x0026d008, guest::houseVisitedMenuEntry)

}  // namespace guest
