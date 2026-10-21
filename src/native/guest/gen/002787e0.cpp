// guest:0x002787e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceCdSyncS(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
  }
  long lVar1;
  undefined8 uVar2;
  
  if (param_1 == 0) {
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      RT_CALLRA(0x278800, guest::scePrintf(0x331c88));
    }
    while( true ) {
      lVar1 = RT_CALLRA(0x278818, guest::sceSifCheckStatRpc(0x2e8948));
      uVar2 = 0;
      if (lVar1 == 0) break;
      RT_CALLRA(0x278810, guest::sceCdDelayThread(0x3c));
    }
  }
  else {
    uVar2 = RT_CALLRA(0x278834, guest::sceSifCheckStatRpc(0x2e8948));
  }
  return uVar2;
}


RT_GUEST_FN(0x002787e0, guest::sceCdSyncS)

}  // namespace guest
