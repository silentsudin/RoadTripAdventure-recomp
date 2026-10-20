// guest:0x0027e900 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __malloc_lock(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  
  iVar1 = GetThreadId();
  if ((*(int32_t*)rtguest::G(0x2e9120)) != iVar1) {
    WaitSema((*(uint32_t*)rtguest::G(0x2e99a8)));
    (*(int32_t*)rtguest::G(0x2e9120)) = iVar1;
  }
  (*(int32_t*)rtguest::G(0x2e9124)) = (*(int32_t*)rtguest::G(0x2e9124)) + 1;
  return 0;
}


RT_GUEST_FN(0x0027e900, guest::__malloc_lock)

}  // namespace guest
