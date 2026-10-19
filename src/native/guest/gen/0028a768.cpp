// guest:0x0028a768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDclose(uint32_t q0, ...) {
uint32_t auStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t aiStack_70 = rtguest::allocFrame(16);
  int iVar1;
  long lVar2;
  undefined8 uVar3;
uint32_t puVar4;
  
  lVar2 = get_iob();
  _sceFsWaitS(10);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    _sceFsSigSema();
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -1;
  }
  else if ((lVar2 == 0) || (puVar4 = (uint32_t)lVar2, (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) == 0)) {
    _sceFsSigSema();
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -9;
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0;
    uVar3 = CreateSema(auStack_90);
    (*(uint32_t*)rtguest::G(0x17788c4)) = aiStack_70;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    lVar2 = sceSifCallRpc(0x1779b80,10,0,0x17788c0,0x14,0x1779500,4,0);
    if (lVar2 < 0) {
      DeleteSema(uVar3);
      _sceFsSigSema();
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
      iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
      _sceFsSigSema();
      if (iVar1 == 0) {
        DeleteSema(uVar3);
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
      }
      else {
        WaitSema(uVar3);
        DeleteSema(uVar3);
        if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4))) {
          (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = 0;
        }
      }
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028a768, guest::sceDclose)

}  // namespace guest
