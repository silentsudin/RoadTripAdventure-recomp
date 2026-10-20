// guest:0x002856f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysStubEio2(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
uint32_t puVar1;
  
  puVar1 = (uint32_t)getReent();
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 5;
  return 0xffffffff;
}


RT_GUEST_FN(0x002856f8, guest::sysStubEio2)

}  // namespace guest
