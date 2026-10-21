// guest:0x0027e900 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __malloc_lock(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = RT_CALLRA(0x27e910, guest::GetThreadId());
  if ((*(int32_t*)rtguest::G(0x2e9120)) != iVar1) {
    RT_CALLRA(0x27e948, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e99a8))));
    (*(int32_t*)rtguest::G(0x2e9120)) = iVar1;
  }
  (*(int32_t*)rtguest::G(0x2e9124)) = (*(int32_t*)rtguest::G(0x2e9124)) + 1;
  return 0;
}


RT_GUEST_FN(0x0027e900, guest::__malloc_lock)

}  // namespace guest
