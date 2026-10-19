// guest:0x0022eaa0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UploadVifPacketB(uint32_t param_1, ...) {
  (*(uint64_t*)rtguest::G(0x29f590)) = param_1;
  vifDirect(0x29f560,5);
  return 0;
}


RT_GUEST_FN(0x0022eaa0, guest::UploadVifPacketB)

}  // namespace guest
