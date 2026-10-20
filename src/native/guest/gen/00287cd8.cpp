// guest:0x00287cd8 hand-ported from Ghidra pseudocode — int* array indexing scaled x4
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifBindRpc(uint32_t param_1, uint32_t param_2, uint64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
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
  lVar3 = RT_CALLRA(0x287d08, guest::sifAllocRpcPacket(0x1778840));
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
      lVar4 = RT_CALLRA(0x287d48, guest::CreateSema(auStack_70));
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = (int)lVar4;
      if (lVar4 < 0) {
        RT_CALLRA(0x287d58, guest::sifFreeRpcPacket(lVar3));
        uVar2 = 0xfffffffd;
      }
      else {
        lVar4 = RT_CALLRA(0x287d80, guest::sceSifSendCmd(0xffffffff80000009,lVar3,0x40,0,0,0));
        if (lVar4 == 0) {
          RT_CALLRA(0x287d90, guest::sifFreeRpcPacket(lVar3));
          RT_CALLRA(0x287d98, guest::DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
          uVar2 = 0xfffffffe;
        }
        else {
          RT_CALLRA(0x287da8, guest::WaitSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
          RT_CALLRA(0x287db0, guest::DeleteSema((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4))));
          uVar2 = 0;
        }
      }
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)) = -1;
      lVar4 = RT_CALLRA(0x287de0, guest::sceSifSendCmd(0xffffffff80000009,lVar3,0x40,0,0,0));
      uVar2 = 0;
      if (lVar4 == 0) {
        RT_CALLRA(0x287df0, guest::sifFreeRpcPacket(lVar3));
        uVar2 = 0xfffffffe;
      }
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00287cd8, guest::sceSifBindRpc)

}  // namespace guest
