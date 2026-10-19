// guest:0x00258f90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot4(uint32_t q0, ...) {
  MusicRpcPlayBgm(4,0);
  return 0;
}


RT_GUEST_FN(0x00258f90, guest::PlayBgmSlot4)

}  // namespace guest
