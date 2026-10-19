// guest:0x002182b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnCarAtDoor(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
uint32_t puVar2;
uint32_t pcVar3;
  uint uVar4;
  int iVar5;
  float fVar6;
  float fVar7;
  float fVar8;
uint32_t auStack_40 = rtguest::allocFrame(4);
uint32_t auStack_3c = rtguest::allocFrame(12);
  
  iVar5 = (int)param_1;
  if (((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) & 0xff00ff00) == 0x7001f00) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x90))) = 0x43c80000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) = 0x44160000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x94))) = 0x43480000;
  }
  else {
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x21)));
    puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2c0698 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23)))) * 4));
    fVar6 = rtguest::fbits(atan2f(rtguest::f2bits((*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x18))) -
                          (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x10)))),rtguest::f2bits(
                          (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x1c))) -
                          (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x14))))));
    pcVar3 = (*(uint32_t*)rtguest::G(0x335710));
    fVar7 = (fVar6 * 32768.0) / (*(float*)rtguest::G(0x332b78));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x90))) =
         ((*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x10))) +
         (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x18)))) * 0.5;
    fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x1c)));
    fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar2 + (uint32_t)bVar1 * 0x20 + 0x14)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x94))) = 0xc1a00000;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))) = (short)(int)(fVar7 - 16384.0);
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) = (fVar6 + fVar8) * 0.5;
    rtCall(pcVar3, param_2 + 0x90,auStack_40,auStack_3c,
              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23))) * 8))) & 3,param_1);
  }
  uVar4 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23))) * 8)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xa0))) = (uVar4 & 0xf) << 0x18;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xa8))) = (((uVar4 & 0x3f) >> 4) * 2 + (uVar4 & 1)) * 0x2000000;
  return 0;
}


RT_GUEST_FN(0x002182b0, guest::SpawnCarAtDoor)

}  // namespace guest
