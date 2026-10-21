// guest:0x0027dbd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t malloc(uint32_t __size, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::gS[0] = 0x00000000002e8cc0ull;  // walk const
  }
uint32_t pvVar1;
  
  RT_CALLRA(0x27dbf4, guest::__malloc_lock((*(uint32_t*)rtguest::G(0x002e8cc0))));
  pvVar1 = (uint32_t)RT_CALLRA(0x27dc00, guest::_malloc_r((*(uint32_t*)rtguest::G(0x002e8cc0)),__size));
  RT_CALLRA(0x27dc0c, guest::__malloc_unlock((*(uint32_t*)rtguest::G(0x002e8cc0))));
  return pvVar1;
}


RT_GUEST_FN(0x0027dbd8, guest::malloc)

}  // namespace guest
