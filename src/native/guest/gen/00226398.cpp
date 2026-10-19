// guest:0x00226398 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t KickVifPacket(uint32_t q0, ...) {
  vifKick(0x28e730);
  return 0;
}


RT_GUEST_FN(0x00226398, guest::KickVifPacket)

}  // namespace guest
