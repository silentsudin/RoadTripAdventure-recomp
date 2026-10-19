// guest:0x00278ca0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdDiskReady(uint64_t param_1, ...) {
  int iVar1;
  undefined4 uVar2;
  long lVar3;
  
  if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    scePrintf(0x331d08);
  }
  cmd_sem_init();
  iVar1 = PollSema((*(int32_t*)rtguest::G(0x2e6f2c)));
  uVar2 = 6;
  if ((*(int32_t*)rtguest::G(0x2e6f2c)) == iVar1) {
    lVar3 = sceCdSyncS(1);
    if (lVar3 == 0) {
      sceSifInitRpc(0);
      if ((*(int32_t*)rtguest::G(0x2e6f44)) < 0) {
        while( true ) {
          while (lVar3 = sceSifBindRpc(0x17737d0,0xffffffff8000059a,0), lVar3 < 0) {
            if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
              scePrintf(0x331d18);
            }
            iVar1 = 0x100000;
            do {
              iVar1 = iVar1 + -1;
            } while (iVar1 != -1);
          }
          iVar1 = 0x100000;
          if ((*(uint32_t*)rtguest::G(0x17737f4)) != 0) break;
          do {
            iVar1 = iVar1 + -1;
          } while (iVar1 != -1);
        }
        (*(uint32_t*)rtguest::G(0x2e6f44)) = 0;
      }
      (*(uint32_t*)rtguest::G(0x1773810)) = (uint32_t)param_1;
      sceSifWriteBackDCache(0x1773810,4);
      lVar3 = sceSifCallRpc(0x17737d0,0,0,0x1773810,4,0x2e8100,4,0);
      if (-1 < lVar3) {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          scePrintf(0x331d38);
        }
        SignalSema((*(int32_t*)rtguest::G(0x2e6f2c)));
        return 0;
      }
    }
    SignalSema((*(int32_t*)rtguest::G(0x2e6f2c)));
    uVar2 = 6;
    if (param_1 == 8) {
      uVar2 = 0xffffffff;
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00278ca0, guest::sceCdDiskReady)

}  // namespace guest
