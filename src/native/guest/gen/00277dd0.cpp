// guest:0x00277dd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _Cdvd_cbLoop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  do {
    WaitSema((*(uint32_t*)rtguest::G(0x2e6f20)));
    if ((*(int32_t*)rtguest::G(0x2e6f54)) == -1) {
      (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
      (*(int32_t*)rtguest::G(0x2e6f54)) = 0;
      (*(int32_t*)rtguest::G(0x2e6f14)) = 0;
      (*(uint32_t*)rtguest::G(0x177358c)) = 0;
      ExitDeleteThread();
    }
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      scePrintf(0x331ba0,(*(uint32_t*)rtguest::G(0x1773580)),(*(uint32_t*)rtguest::G(0x2e6f58)));
    }
    if (((*(uint32_t*)rtguest::G(0x1773580)) != (uint32_t)0x0) && ((*(uint32_t*)rtguest::G(0x2e6f58)) != 0)) {
      rtCall((*(uint32_t*)rtguest::G(0x1773580)), (*(uint32_t*)rtguest::G(0x2e6f58)));
    }
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
  } while( true );
}


RT_GUEST_FN(0x00277dd0, guest::_Cdvd_cbLoop)

}  // namespace guest
