// guest:0x002789c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdInit(uint32_t param_1, ...) {
  int iVar1;
  long lVar2;
  undefined4 uVar3;
  
  lVar2 = sceCdSyncS(1);
  uVar3 = 0;
  if (lVar2 == 0) {
    sceSifInitRpc(0);
    (*(uint32_t*)rtguest::G(0x1773590)) = GetThreadId();
    (*(uint32_t*)rtguest::G(0x2e6f24)) = 1;
    (*(int32_t*)rtguest::G(0x2e6f50)) = (*(int32_t*)rtguest::G(0x2e6f50)) + 1;
    (*(int32_t*)rtguest::G(0x2e6f3c)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f40)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f38)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f48)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f44)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f34)) = 0;
    (*(uint32_t*)rtguest::G(0x2e6f4c)) = 0xffffffff;
    while( true ) {
      while (lVar2 = sceSifBindRpc(0x17737a8,0xffffffff80000592,0), lVar2 < 0) {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          scePrintf(0x331cd8,lVar2,(*(int32_t*)rtguest::G(0x2e6f50)));
        }
        iVar1 = 0x100000;
        do {
          iVar1 = iVar1 + -1;
        } while (iVar1 != -1);
      }
      if ((*(uint32_t*)rtguest::G(0x17737cc)) != 0) break;
      iVar1 = 0x100000;
      do {
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    (*(uint32_t*)rtguest::G(0x2e6f4c)) = 0;
    (*(uint32_t*)rtguest::G(0x1773800)) = param_1;
    sceSifWriteBackDCache(0x1773800,4);
    lVar2 = sceSifCallRpc(0x17737a8,0,0,0x1773800,4,0x2e8100,0x10,0);
    if (lVar2 < 0) {
      (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
      uVar3 = 0;
    }
    else {
      uVar3 = 2;
      (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
      if (((param_1 < 0) || (param_1 < 2)) || (param_1 != 5)) {
        cmd_sem_init();
        PowerOffCB();
      }
      else {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          scePrintf(0x331cf8);
        }
        cdvd_exit();
        (*(int32_t*)rtguest::G(0x2e6f28)) = 0xffffffff;
        (*(int32_t*)rtguest::G(0x2e6f2c)) = 0xffffffff;
        (*(uint32_t*)rtguest::G(0x2e6f20)) = 0xffffffff;
      }
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x002789c0, guest::sceCdInit)

}  // namespace guest
