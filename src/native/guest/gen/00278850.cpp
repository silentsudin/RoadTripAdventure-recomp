// guest:0x00278850 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_scmd_prechk(uint32_t param_1, ...) {
  int iVar1;
  long lVar2;
  
  cmd_sem_init();
  iVar1 = PollSema((*(int32_t*)rtguest::G(0x2e6f2c)));
  if ((*(int32_t*)rtguest::G(0x2e6f2c)) == iVar1) {
    (*(uint32_t*)rtguest::G(0x2e6f18)) = (uint32_t)param_1;
    ReferThreadStatus((*(uint32_t*)rtguest::G(0x1773590)),0x1773598);
    lVar2 = sceCdSyncS(1);
    if (lVar2 == 0) {
      sceSifInitRpc(0);
      if (-1 < (*(uint32_t*)rtguest::G(0x2e6f48))) {
        return 1;
      }
      while( true ) {
        while (lVar2 = sceSifBindRpc(0x2e8948,0xffffffff80000593,0), lVar2 < 0) {
          if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
            scePrintf(0x331cc0);
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
    }
    SignalSema((*(int32_t*)rtguest::G(0x2e6f2c)));
  }
  else if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    scePrintf(0x331c98,param_1,(*(uint32_t*)rtguest::G(0x2e6f18)));
    return 0;
  }
}


RT_GUEST_FN(0x00278850, guest::_sceCd_scmd_prechk)

}  // namespace guest
