// guest:0x002780a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdvd_exit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  if ((*(int32_t*)rtguest::G(0x2e6f14)) != 0) {
    (*(int32_t*)rtguest::G(0x2e6f54)) = 0xffffffff;
    RT_CALLRA(0x2780c8, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e6f20))));
  }
  RT_CALLRA(0x2780e0, guest::DeleteSema((*(int32_t*)rtguest::G(0x2e6f28))));
  RT_CALLRA(0x2780ec, guest::DeleteSema((*(int32_t*)rtguest::G(0x2e6f2c))));
  RT_CALLRA(0x2780f4, guest::DeleteSema((*(uint32_t*)rtguest::G(0x2e6f20))));
  RT_CALLRA(0x2780fc, guest::DIntr());
  RT_CALLRA(0x278108, guest::sceSifRemoveCmdHandler(0xffffffff80000012));
  EIntr();
  return 0;
}


RT_GUEST_FN(0x002780a0, guest::cdvd_exit)

}  // namespace guest
