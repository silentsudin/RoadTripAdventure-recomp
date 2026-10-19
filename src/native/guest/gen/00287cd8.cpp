// guest:0x00287cd8 hand-ported from Ghidra pseudocode — int* array indexing scaled x4
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifBindRpc(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {
  int iVar1;
  undefined4 uVar2;
  long lVar3;
  long lVar4;
  int iVar5;
  uint32_t auStack_70 = rtguest::allocFrame(12);
  uint32_t uStack_6c = auStack_70 + 4;
  uint32_t uStack_68 = auStack_70 + 8;

  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * 4)) = 0;
  lVar3 = sifAllocRpcPacket(0x1778840);
  uVar2 = 0xffffffff;
  if (lVar3 != 0) {
    iVar5 = (int)lVar3;
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = iVar5;
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * 4)) = iVar1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = param_2;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) = iVar5;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) = param_1;
    if ((param_3 & 1) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 1;
      lVar4 = CreateSema(auStack_70);
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = (int)lVar4;
      if (lVar4 < 0) {
        sifFreeRpcPacket(lVar3);
        uVar2 = 0xfffffffd;
      }
      else {
        lVar4 = sceSifSendCmd(0xffffffff80000009,lVar3,0x40,0,0,0);
        if (lVar4 == 0) {
          sifFreeRpcPacket(lVar3);
          DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)));
          uVar2 = 0xfffffffe;
        }
        else {
          WaitSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)));
          DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)));
          uVar2 = 0;
        }
      }
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = -1;
      lVar4 = sceSifSendCmd(0xffffffff80000009,lVar3,0x40,0,0,0);
      uVar2 = 0;
      if (lVar4 == 0) {
        sifFreeRpcPacket(lVar3);
        uVar2 = 0xfffffffe;
      }
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00287cd8, guest::sceSifBindRpc)

}  // namespace guest
