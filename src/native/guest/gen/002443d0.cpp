// guest:0x002443d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildCarouselQuadA(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  short sVar1;
  int iVar2;
uint32_t puVar3;
  uint uVar4;
  short sVar5;
  int iVar6;
  float fVar7;
  
  fVar7 = 0.0;
  param_2 = param_2 + param_3 * 0xd0;
  if (param_1 == 0.0) {
    iVar2 = 0x80;
  }
  else {
    cos((double)(long)param_2);
    iVar2 = (int)(fVar7 * 16.0) + 0x60;
  }
  puVar3 = (uint32_t)(param_2 + 0x4c);
  iVar6 = 3;
  do {
    iVar6 = iVar6 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-1) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-2) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-3) * sizeof(uint32_t))) = iVar2;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0x80;
    puVar3 = puVar3 + 0xc;
  } while (-1 < iVar6);
  iVar2 = param_6 * 0x80;
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xff;
  }
  iVar6 = param_6 * 0xd8;
  uVar4 = param_4 - (iVar2 >> 8);
  if (iVar6 < 0) {
    iVar6 = iVar6 + 0xff;
  }
  sVar1 = param_5 + (short)((uint32_t)iVar6 >> 8);
  sVar5 = (short)uVar4;
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = sVar5 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))) = param_5 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x54))) = param_5 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = ((int)sVar5 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  param_6 = param_6 << 4;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x58))) = param_6;
  iVar2 = (int)(uVar4 & 0xffff | (param_4 + (iVar2 >> 8)) * 0x10000) >> 0x10;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x80))) = iVar2 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x84))) = param_5 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x84))) = param_5 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x80))) = (iVar2 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x88))) = param_6;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb0))) = sVar5 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb4))) = sVar1 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb4))) = sVar1 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb0))) = ((int)sVar5 << 6) / 5 + 0x6ff8;
  }
  iVar6 = (*(uint32_t*)rtguest::G(0x333288));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xb8))) = param_6;
  if (iVar6 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe0))) = iVar2 * 0x10 + 0x6bf8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe4))) = sVar1 * 8 + 0x78f8;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe4))) = sVar1 * 8 + 0x77f8;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe0))) = (iVar2 << 6) / 5 + 0x6ff8;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xe8))) = param_6;
  return 0;
}


RT_GUEST_FN(0x002443d0, guest::BuildCarouselQuadA)

}  // namespace guest
