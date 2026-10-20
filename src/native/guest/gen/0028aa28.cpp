// guest:0x0028aa28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGetstat(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(16);
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  
  _sceFsWaitS(0xc);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
    (*(uint32_t*)rtguest::G(0x17788d0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788d0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar2 = 0;
  cVar1 = (*(uint32_t*)rtguest::G(0x17788d0));
  while (iVar5 = iVar2, cVar1 != '\0') {
    iVar5 = iVar2 + 1;
    if (0x3ff < iVar5) break;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar5) * sizeof(int8_t)));
    (*(uint32_t*)rtguest::G((uint32_t)0x17788d1 + (uint32_t)(iVar2) * 4)) = cVar1;
    iVar2 = iVar5;
  }
  if (iVar5 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778ccf)) = 0;
    iVar5 = 0x3ff;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  (*(uint32_t*)rtguest::G(0x17788cc)) = param_2;
  uVar3 = CreateSema(auStack_c0);
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
  lVar4 = sceSifCallRpc(0x1779b80,0xc,0,0x17788c0,iVar5 + 0x11,0x1779500,4,0);
  iVar2 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar4 < 0) {
    DeleteSema(uVar3);
    _sceFsSigSema();
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    _sceFsSigSema();
    if (iVar2 == 0) {
      DeleteSema(uVar3);
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      WaitSema(uVar3);
      DeleteSema(uVar3);
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028aa28, guest::sceGetstat)

}  // namespace guest
