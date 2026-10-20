// guest:0x002878d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifFreeRpcPacket(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) & 0xfffffffe;
  return 0;
}


RT_GUEST_FN(0x002878d8, guest::sifFreeRpcPacket)

}  // namespace guest
