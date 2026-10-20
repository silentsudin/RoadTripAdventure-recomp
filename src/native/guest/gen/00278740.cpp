// guest:0x00278740 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdSync(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  undefined4 uVar1;
  long lVar2;
  
  if (param_1 == 0) {
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      RT_CALLRA(0x278764, guest::scePrintf(0x331c78));
    }
    while (((*(uint32_t*)rtguest::G(0x2e6f30)) != 0 || (lVar2 = sceSifCheckStatRpc(0x2e80d0), lVar2 != 0))) {
      RT_CALLRA(0x278778, guest::sceCdDelayThread(0x3c));
    }
    return 0;
  }
  uVar1 = 1;
  if ((*(uint32_t*)rtguest::G(0x2e6f30)) == 0) {
    lVar2 = sceSifCheckStatRpc(0x2e80d0);
    uVar1 = 1;
    if (lVar2 == 0) {
      uVar1 = 0;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x00278740, guest::sceCdSync)

}  // namespace guest
