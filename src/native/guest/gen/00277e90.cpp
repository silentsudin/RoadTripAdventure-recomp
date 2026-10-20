// guest:0x00277e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdInitEeCB(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  undefined8 uVar2;
  
  bVar1 = (*(int32_t*)rtguest::G(0x2e6f14)) != 0;
  if (bVar1) {
    ChangeThreadPriority((*(int32_t*)rtguest::G(0x2e6f14)),param_1);
  }
  else {
    uVar2 = GetThreadId();
    (*(uint32_t*)rtguest::G(0x1773590)) = (uint32_t)uVar2;
    ReferThreadStatus(uVar2,0x1773598);
    (*(uint32_t*)rtguest::G(0x17735d8)) = 0x33aaf0;
    (*(uint32_t*)rtguest::G(0x17735cc)) = 0x277dd0;
    (*(uint32_t*)rtguest::G(0x17735dc)) = (uint32_t)param_1;
    (*(uint32_t*)rtguest::G(0x17735d0)) = param_2;
    (*(uint32_t*)rtguest::G(0x17735d4)) = param_3;
    uVar2 = CreateThread(0x17735c8);
    (*(int32_t*)rtguest::G(0x2e6f14)) = (int)uVar2;
    StartThread(uVar2,0);
  }
  return !bVar1;
}


RT_GUEST_FN(0x00277e90, guest::sceCdInitEeCB)

}  // namespace guest
