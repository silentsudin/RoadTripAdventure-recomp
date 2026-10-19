// guest:0x00287218 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifExitCmd(uint32_t q0, ...) {
  disableDmac(5);
  RemoveDmacHandler(5,(*(uint32_t*)rtguest::G(0x1776e94)));
  (*(int32_t*)rtguest::G(0x2e98fc)) = 0;
  return 0;
}


RT_GUEST_FN(0x00287218, guest::sceSifExitCmd)

}  // namespace guest
