// guest:0x0028a560 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceAddDrv(uint32_t param_1, ...) {
uint32_t auStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_6c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_60 = rtguest::allocFrame(16);
  int iVar1;
  undefined8 uVar2;
  long lVar3;
  
  _sceFsWaitS(0xf);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
  (*(uint32_t*)rtguest::G(0x17788cc)) = param_1;
  uVar2 = CreateSema(auStack_80);
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_60;
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  lVar3 = sceSifCallRpc(0x1779b80,0xf,0,0x17788c0,0x10,0x1779500,4,0);
  iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    DeleteSema(uVar2);
    _sceFsSigSema();
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xffffffff;
  }
  else {
    _sceFsSigSema();
    if (iVar1 == 0) {
      DeleteSema(uVar2);
      (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xffffffff;
    }
    else {
      WaitSema(uVar2);
      DeleteSema(uVar2);
    }
  }

}


RT_GUEST_FN(0x0028a560, guest::sceAddDrv)

}  // namespace guest
