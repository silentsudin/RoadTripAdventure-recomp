// guest:0x00277e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdInitEeCB(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x0000000000000001ull;  // walk const
    rtguest::gS[4] = 0x00000000002e0000ull;  // walk const
  }
  bool bVar1;
  undefined8 uVar2;
  
  bVar1 = (*(int32_t*)rtguest::G(0x2e6f14)) != 0;
  if (bVar1) {
    RT_CALLRA(0x277f38, guest::ChangeThreadPriority((*(int32_t*)rtguest::G(0x2e6f14)),param_1));
  }
  else {
    uVar2 = RT_CALLRA(0x277ec8, guest::GetThreadId());
    (*(uint32_t*)rtguest::G(0x1773590)) = (uint32_t)uVar2;
    RT_CALLRA(0x277ee0, guest::ReferThreadStatus(uVar2,0x1773598));
    (*(uint32_t*)rtguest::G(0x17735d8)) = 0x33aaf0;
    (*(uint32_t*)rtguest::G(0x17735cc)) = 0x277dd0;
    (*(uint32_t*)rtguest::G(0x17735dc)) = (uint32_t)param_1;
    (*(uint32_t*)rtguest::G(0x17735d0)) = param_2;
    (*(uint32_t*)rtguest::G(0x17735d4)) = param_3;
    uVar2 = RT_CALLRA(0x277f14, guest::CreateThread(0x17735c8));
    (*(int32_t*)rtguest::G(0x2e6f14)) = (int)uVar2;
    RT_CALLRA(0x277f24, guest::StartThread(uVar2,0));
  }
  return !bVar1;
}


RT_GUEST_FN(0x00277e90, guest::sceCdInitEeCB)

}  // namespace guest
