// guest:0x00277dd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Cdvd_cbLoop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
    rtguest::gS[0] = 0x00000000002e0000ull;  // walk const
    rtguest::gS[1] = 0x0000000001770000ull;  // walk const
    rtguest::gS[2] = 0x00000000002e0000ull;  // walk const
    rtguest::gS[3] = 0x00000000002e0000ull;  // walk const
    rtguest::gS[4] = 0x0000000000330000ull;  // walk const
    rtguest::gS[5] = 0x00000000002e0000ull;  // walk const
    rtguest::gS[6] = 0x0000000001770000ull;  // walk const
    rtguest::gS[7] = 0x00000000002e0000ull;  // walk const
    rtguest::gS[8] = 0x00000000002e0000ull;  // walk const
  }
  do {
    RT_CALLRA(0x277e20, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e6f20))));
    if ((*(int32_t*)rtguest::G(0x2e6f54)) == -1) {
      (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
      (*(int32_t*)rtguest::G(0x2e6f54)) = 0;
      (*(int32_t*)rtguest::G(0x2e6f14)) = 0;
      (*(uint32_t*)rtguest::G(0x177358c)) = 0;
      RT_CALLRA(0x277e44, guest::ExitDeleteThread());
    }
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      RT_CALLRA(0x277e5c, guest::scePrintf(0x331ba0,(*(uint32_t*)rtguest::G(0x1773580)),(*(uint32_t*)rtguest::G(0x2e6f58))));
    }
    if (((*(uint32_t*)rtguest::G(0x1773580)) != (uint32_t)0x0) && ((*(uint32_t*)rtguest::G(0x2e6f58)) != 0)) {
      RT_CALLRA(0x00277e7c, rtCall((*(uint32_t*)rtguest::G(0x1773580)), (*(uint32_t*)rtguest::G(0x2e6f58))));
    }
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
  } while( true );
}


RT_GUEST_FN(0x00277dd0, guest::_Cdvd_cbLoop)

}  // namespace guest
