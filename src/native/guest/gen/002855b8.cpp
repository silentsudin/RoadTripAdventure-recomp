// guest:0x002855b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sysStubEio(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
uint32_t puVar1;
  
  puVar1 = (uint32_t)RT_CALLRA(0x2855c0, guest::getReent());
  (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 5;
  return 0xffffffff;
}


RT_GUEST_FN(0x002855b8, guest::sysStubEio)

}  // namespace guest
