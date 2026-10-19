// guest:0x00258fd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PlayBgmSlot6(uint32_t q0, ...) {
  MusicRpcPlayBgm(6,0);
  return 0;
}


RT_GUEST_FN(0x00258fd0, guest::PlayBgmSlot6)

}  // namespace guest
