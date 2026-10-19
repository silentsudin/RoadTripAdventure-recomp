// guest:0x002793d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSdRemote(uint64_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  uint32_t uStack_30 = rtguest::allocFrame(0x38);
  uint32_t uStack_28 = uStack_30 + 8;
  uint32_t uStack_20 = uStack_30 + 0x10;
  uint32_t uStack_18 = uStack_30 + 0x18;
  uint32_t uStack_10 = uStack_30 + 0x20;
  uint32_t uStack_8 = uStack_30 + 0x28;
  bool bVar1;
uint32_t puVar2;
  int iVar3;
uint32_t puVar4;
uint32_t puVar5;
  undefined8 uVar6;
  undefined4 uVar7;
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  puVar4 = 0x01773984;
  iVar3 = 5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  (*(uint32_t*)rtguest::G(0x1773980)) = 0x01773980;
  puVar2 = uStack_30;
  do {
    iVar3 = iVar3 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar2));
    puVar4 = puVar4 + 0x4;  // T* element step
    puVar2 = puVar2 + 0x8;  // T* element step
  } while (-1 < iVar3);
  bVar1 = param_1 == 0;
  uVar7 = 0;
  if (bVar1) {
    uVar7 = (*(uint32_t*)rtguest::G(0x002e8980));
  }
  if (param_2 == 0x8160) {
    if ((*(uint32_t*)rtguest::G(0x1773984)) == 0) {
      (*(uint32_t*)rtguest::G(0x2e899c)) = (*(uint32_t*)rtguest::G(0x177398c));
      (*(uint32_t*)rtguest::G(0x2e8990)) = (*(uint32_t*)rtguest::G(0x1773988));
    }
    else {
      (*(uint32_t*)rtguest::G(0x2e89a0)) = (*(uint32_t*)rtguest::G(0x177398c));
      (*(uint32_t*)rtguest::G(0x2e8994)) = (*(uint32_t*)rtguest::G(0x1773988));
    }
  }
  else if (param_2 == 0x8170) {
    (*(uint32_t*)rtguest::G(0x2e89a4)) = (*(uint32_t*)rtguest::G(0x1773988));
    (*(uint32_t*)rtguest::G(0x2e8998)) = (*(uint32_t*)rtguest::G(0x1773984));
  }
  if (param_2 == 0x8130) {
    param_2 = (*(uint32_t*)rtguest::G(0x1773984)) | 0x8130;
    uVar6 = 0;
    puVar4 = (*(uint32_t*)rtguest::G(0x1773988));
    puVar5 = (uint32_t)0x0;
  }
  else {
    puVar4 = 0x01773980;
    if (param_2 == 0x8140) {
      sceSifCallRpc(0x17739c0,(*(uint32_t*)rtguest::G(0x1773984)) | 0x8140,bVar1,0x1773980,0x40,(*(uint32_t*)rtguest::G(0x1773988)),0x40,uVar7);
      return 0;
    }
    uVar6 = 0x10;
    puVar5 = puVar4;
  }
  sceSifCallRpc(0x17739c0,param_2,bVar1,puVar4,0x40,puVar5,uVar6,uVar7);
  return 0;
}


RT_GUEST_FN(0x002793d0, guest::sceSdRemote)

}  // namespace guest
