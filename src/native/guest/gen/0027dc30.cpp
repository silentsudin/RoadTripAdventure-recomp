// guest:0x0027dc30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t free(uint32_t __ptr, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  RT_CALLRA(0x27dc4c, guest::__malloc_lock((*(uint32_t*)rtguest::G(0x002e8cc0))));
  RT_CALLRA(0x27dc58, guest::_free_r((*(uint32_t*)rtguest::G(0x002e8cc0)),__ptr));
  __malloc_unlock((*(uint32_t*)rtguest::G(0x002e8cc0)));
  return 0;
}


RT_GUEST_FN(0x0027dc30, guest::free)

}  // namespace guest
