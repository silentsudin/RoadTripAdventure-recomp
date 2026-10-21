// guest:0x00277dd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Cdvd_cbLoop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
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
