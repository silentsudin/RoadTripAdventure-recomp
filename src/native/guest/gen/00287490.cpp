// guest:0x00287490 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceSifCmdIntrHdlr(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(4);
  byte bVar1;
uint32_t pcVar2;
  undefined8 uVar3;
uint32_t puVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
uint32_t pbVar8;
  
  puVar4 = auStack_90;
  EIntr();
  pbVar8 = (*(uint32_t*)rtguest::G(0x1776e98));
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1776e98))));
  if (bVar1 != 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x1776e98)))) = 0;
    for (iVar7 = (int)(bVar1 + 0xf) >> 4; iVar7 != 0; iVar7 = iVar7 + -1) {
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pbVar8));
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar8 + 8)));
      uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(pbVar8 + 0xc)));
      pbVar8 = pbVar8 + 0x10;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = (int)uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) = (int32_t)(uVar3 >> 0x20);
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint32_t))) = uVar5;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint32_t))) = uVar6;
      puVar4 = puVar4 + 0x10;  // T* element step
    }
    isceSifSetDChain();
    if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_88)) < 0) {
      if ((int)((*(uint32_t*)rtguest::G((uint32_t)uStack_88)) & 0x7fffffff) < (*(uint32_t*)rtguest::G(0x1776ea8))) {
        puVar4 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)uStack_88)) * 8 + (*(int32_t*)rtguest::G(0x1776ea4)));
        pcVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar4));
        if (pcVar2 != (uint32_t)0x0) {
          rtCall(pcVar2, auStack_90,(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))));
        }
      }
    }
    else if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_88)) < (*(uint32_t*)rtguest::G(0x1776eb0))) {
      puVar4 = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)uStack_88)) * 8 + (*(uint32_t*)rtguest::G(0x1776eac)));
      pcVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar4));
      if (pcVar2 != (uint32_t)0x0) {
        rtCall(pcVar2, auStack_90,(*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))));
      }
    }
    SYNC(0);
    EI();
  }
  return 0;
}


RT_GUEST_FN(0x00287490, guest::_sceSifCmdIntrHdlr)

}  // namespace guest
