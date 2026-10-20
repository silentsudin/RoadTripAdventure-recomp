// guest:0x00278538 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_ncmd_prechk(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  int iVar1;
  long lVar2;
  
  RT_CALLRA(0x278550, guest::cmd_sem_init());
  iVar1 = RT_CALLRA(0x27855c, guest::PollSema((*(int32_t*)rtguest::G(0x2e6f28))));
  if ((*(int32_t*)rtguest::G(0x2e6f28)) == iVar1) {
    (*(uint32_t*)rtguest::G(0x2e6f1c)) = (uint32_t)param_1;
    RT_CALLRA(0x2785ac, guest::ReferThreadStatus((*(uint32_t*)rtguest::G(0x1773590)),0x1773598));
    lVar2 = RT_CALLRA(0x2785b4, guest::sceCdSync(1));
    if (lVar2 == 0) {
      RT_CALLRA(0x2785d8, guest::sceSifInitRpc(0));
      if (-1 < (*(int32_t*)rtguest::G(0x2e6f38))) {
        return 1;
      }
      while( true ) {
        while (lVar2 = sceSifBindRpc(0x2e80d0,0xffffffff80000595,0), lVar2 < 0) {
          if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
            RT_CALLRA(0x278588, guest::scePrintf(0x331c60));
          }
          iVar1 = 0x100000;
          do {
            iVar1 = iVar1 + -1;
          } while (iVar1 != -1);
        }
        if ((*(uint32_t*)rtguest::G(0x2e80f4)) != 0) break;
        iVar1 = 0x100000;
        do {
          iVar1 = iVar1 + -1;
        } while (iVar1 != -1);
      }
      (*(uint32_t*)rtguest::G(0x2e6f38)) = 0;
      return 1;
    }
    RT_CALLRA(0x2785c8, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f28))));
  }
  else if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    RT_CALLRA(0x278650, guest::scePrintf(0x331c38,param_1,(*(uint32_t*)rtguest::G(0x2e6f1c))));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00278538, guest::_sceCd_ncmd_prechk)

}  // namespace guest
