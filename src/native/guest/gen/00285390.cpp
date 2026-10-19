// guest:0x00285390 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t isceSifSetDChain(uint32_t q0, ...) {
  syscall(0xffffffffffffff88);
  return 0;
}


RT_GUEST_FN(0x00285390, guest::isceSifSetDChain)

}  // namespace guest
