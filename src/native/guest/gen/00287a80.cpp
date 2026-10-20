// guest:0x00287a80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifGetOtherData(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint64_t param_5, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
  uint32_t uVar2;
  uint32_t lVar3;
  uint32_t lVar4;
  uint32_t iVar5;
  uint32_t iVar1;
  uint32_t auStack_90 = rtguest::allocFrame(12);  // auStack_90[4] + uStack_8c + uStack_88

  lVar3 = RT_CALLRA(0x287ab8, guest::sifAllocRpcPacket(0x1778840));
  uVar2 = 0xffffffff;
  if (lVar3 != 0) {
    iVar5 = lVar3;
    iVar1 = *(int32_t*)rtguest::G((uint32_t)(iVar5 + 0x18));
    *(uint32_t*)rtguest::G((uint32_t)param_1) = iVar5;
    *(uint32_t*)rtguest::G((uint32_t)(param_1 + 4)) = (uint32_t)iVar1;
    *(uint32_t*)rtguest::G((uint32_t)(iVar5 + 0x20)) = param_2;
    *(uint32_t*)rtguest::G((uint32_t)(iVar5 + 0x24)) = param_3;
    *(uint32_t*)rtguest::G((uint32_t)(iVar5 + 0x28)) = param_4;
    *(uint32_t*)rtguest::G((uint32_t)(iVar5 + 0x14)) = iVar5;
    *(uint32_t*)rtguest::G((uint32_t)(iVar5 + 0x1c)) = param_1;
    if ((param_5 & 1) == 0) {
      *(uint32_t*)rtguest::G((uint32_t)(auStack_90 + 8)) = 0;  // uStack_88
      *(uint32_t*)rtguest::G((uint32_t)(auStack_90 + 4)) = 1;  // uStack_8c
      lVar4 = RT_CALLRA(0x287b00, guest::CreateSema(auStack_90));
      *(uint32_t*)rtguest::G((uint32_t)(param_1 + 8)) = lVar4;
      if ((int32_t)lVar4 < 0) {
        RT_CALLRA(0x287b10, guest::sifFreeRpcPacket(lVar3));
        uVar2 = 0xfffffffd;
      }
      else {
        lVar4 = RT_CALLRA(0x287b38, guest::sceSifSendCmd(0x8000000c,lVar3,0x40,0,0,0));
        if (lVar4 == 0) {
          RT_CALLRA(0x287b48, guest::sifFreeRpcPacket(lVar3));
          RT_CALLRA(0x287b50, guest::DeleteSema(*(uint32_t*)rtguest::G((uint32_t)(param_1 + 8))));
          uVar2 = 0xfffffffe;
        }
        else {
          RT_CALLRA(0x287b60, guest::WaitSema(*(uint32_t*)rtguest::G((uint32_t)(param_1 + 8))));
          RT_CALLRA(0x287b68, guest::DeleteSema(*(uint32_t*)rtguest::G((uint32_t)(param_1 + 8))));
          uVar2 = 0;
        }
      }
    }
    else {
      *(uint32_t*)rtguest::G((uint32_t)(param_1 + 8)) = 0xffffffff;
      lVar4 = RT_CALLRA(0x287b98, guest::sceSifSendCmd(0x8000000c,lVar3,0x40,0,0,0));
      uVar2 = 0;
      if (lVar4 == 0) {
        RT_CALLRA(0x287ba8, guest::sifFreeRpcPacket(lVar3));
        uVar2 = 0xfffffffe;
      }
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00287a80, guest::sceSifGetOtherData)

}  // namespace guest
