// guest:0x00278850 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_scmd_prechk(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::gS[0] = 0x00000000002e0000ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  long lVar2;
  
  RT_CALLRA(0x278868, guest::cmd_sem_init());
  iVar1 = RT_CALLRA(0x278874, guest::PollSema((*(int32_t*)rtguest::G(0x2e6f2c))));
  if ((*(int32_t*)rtguest::G(0x2e6f2c)) == iVar1) {
    (*(uint32_t*)rtguest::G(0x2e6f18)) = (uint32_t)param_1;
    RT_CALLRA(0x2788c4, guest::ReferThreadStatus((*(uint32_t*)rtguest::G(0x1773590)),0x1773598));
    lVar2 = RT_CALLRA(0x2788cc, guest::sceCdSyncS(1));
    if (lVar2 == 0) {
      RT_CALLRA(0x2788f0, guest::sceSifInitRpc(0));
      if (-1 < (*(int32_t*)rtguest::G(0x2e6f48))) {
        return 1;
      }
      while( true ) {
        while (lVar2 = sceSifBindRpc(0x2e8948,0xffffffff80000593,0), lVar2 < 0) {
          if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
            RT_CALLRA(0x2788a0, guest::scePrintf(0x331cc0));
          }
          iVar1 = 0x100000;
          do {
            iVar1 = iVar1 + -1;
          } while (iVar1 != -1);
        }
        if ((*(uint32_t*)rtguest::G(0x2e896c)) != 0) break;
        iVar1 = 0x100000;
        do {
          iVar1 = iVar1 + -1;
        } while (iVar1 != -1);
      }
      (*(uint32_t*)rtguest::G(0x2e6f48)) = 0;
      return 1;
    }
    RT_CALLRA(0x2788e0, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
  }
  else if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    RT_CALLRA(0x278968, guest::scePrintf(0x331c98,param_1,(*(uint32_t*)rtguest::G(0x2e6f18))));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00278850, guest::_sceCd_scmd_prechk)

}  // namespace guest
