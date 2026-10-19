// guest:0x00278538 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_ncmd_prechk(uint32_t param_1, ...) {
  int iVar1;
  long lVar2;
  
  cmd_sem_init();
  iVar1 = PollSema((*(int32_t*)rtguest::G(0x2e6f28)));
  if ((*(int32_t*)rtguest::G(0x2e6f28)) == iVar1) {
    (*(uint32_t*)rtguest::G(0x2e6f1c)) = (uint32_t)param_1;
    ReferThreadStatus((*(uint32_t*)rtguest::G(0x1773590)),0x1773598);
    lVar2 = sceCdSync(1);
    if (lVar2 == 0) {
      sceSifInitRpc(0);
      if (-1 < (*(uint32_t*)rtguest::G(0x2e6f38))) {
        return 1;
      }
      while( true ) {
        while (lVar2 = sceSifBindRpc(0x2e80d0,0xffffffff80000595,0), lVar2 < 0) {
          if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
            scePrintf(0x331c60);
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
    SignalSema((*(int32_t*)rtguest::G(0x2e6f28)));
  }
  else if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    scePrintf(0x331c38,param_1,(*(uint32_t*)rtguest::G(0x2e6f1c)));
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00278538, guest::_sceCd_ncmd_prechk)

}  // namespace guest
