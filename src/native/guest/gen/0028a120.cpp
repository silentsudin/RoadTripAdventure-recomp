// guest:0x0028a120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMkdir(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(16);
  char cVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  _sceFsWaitS(7);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
    (*(uint32_t*)rtguest::G(0x17788d0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788d0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar4 = 0;
  if ((*(uint32_t*)rtguest::G(0x17788d0)) != '\0') {
    for (iVar4 = 1;
        (iVar4 < 0x400 && (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x17788d0 + (uint32_t)(iVar4) * 4)) = cVar1, cVar1 != '\0'));
        iVar4 = iVar4 + 1) {
    }
  }
  if (iVar4 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778ccf)) = 0;
    iVar4 = 0x3ff;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  (*(uint32_t*)rtguest::G(0x17788cc)) = param_2;
  uVar2 = CreateSema(auStack_c0);
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
  lVar3 = sceSifCallRpc(0x1779b80,7,0,0x17788c0,iVar4 + 0x11,0x1779500,4,0);
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    DeleteSema(uVar2);
    _sceFsSigSema();
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    _sceFsSigSema();
    if (iVar4 == 0) {
      DeleteSema(uVar2);
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      WaitSema(uVar2);
      DeleteSema(uVar2);
    }
  }

}


RT_GUEST_FN(0x0028a120, guest::sceMkdir)

}  // namespace guest
