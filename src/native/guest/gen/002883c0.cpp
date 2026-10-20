// guest:0x002883c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifExecRequest(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(128);
  undefined4 uVar1;
  long lVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
uint32_t aiStack_80 = rtguest::allocFrame(32);
  
  iVar5 = 0;
  lVar2 = rtCall((*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))))), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))));
  if (lVar2 != 0) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c)));
  }
  if (0 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc)))) {
    sceSifWriteBackDCache((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
  }
  if (0 < iVar5) {
    sceSifWriteBackDCache(lVar2,iVar5);
  }
  DIntr();
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) & 4) == 0) {
    uVar3 = _sceRpcGetFPacket(0x1778840,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) >> 0x10);
  }
  else {
    uVar3 = _sceRpcGetFPacket2(0x1778840);
  }
  EIntr();
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  iVar6 = (int)uVar3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) = 0x8000000a;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1c))) = uVar1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10))) = 0;
    if (0 < iVar5) {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(1) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
      (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(0) * 4)) = (int)lVar2;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(3) * 4)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(2) * 4)) = iVar5;
    }
    uVar7 = (uint32_t)(0 < iVar5);
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(uVar7 * 4) * 4)) = iVar6;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(uVar7 * 4 + 1) * 4)) = iVar5;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(uVar7 * 4 + 2) * 4)) = 0x40;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_80 + (uint32_t)(uVar7 * 4 + 3) * 4)) = 0;
    while (lVar2 = sceSifSetDma(aiStack_80,uVar7 + 1), lVar2 == 0) {
      iVar5 = 0x100000;
      do {
        iVar5 = iVar5 + -1;
      } while (iVar5 != -1);
    }
  }
  else {
    do {
      lVar4 = sceSifSendCmd(0xffffffff80000008,uVar3,0x40,lVar2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),
                            iVar5);
    } while (lVar4 == 0);
  }
  return 0;
}


RT_GUEST_FN(0x002883c0, guest::sceSifExecRequest)

}  // namespace guest
