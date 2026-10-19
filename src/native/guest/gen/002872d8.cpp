// guest:0x002872d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceSifSendCmd(uint32_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint32_t param_5, uint32_t param_6, uint64_t param_7, ...) {
  undefined8 uVar1;
  int iVar2;
uint32_t puVar3;
  int iVar4;
uint32_t auStack_80 = rtguest::allocFrame(32);
  
  if (0x60 < (int)param_4 - 0x10U) {
    return 0;
  }
  iVar4 = 0;
  puVar3 = (uint32_t)param_3;
  if (param_7 < 1) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)puVar3));
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(2) * 4)) = (uint32_t)param_7;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t))) = param_6;
    iVar4 = 1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)puVar3)) | (*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(2) * 4)) << 8;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(3) * 4)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(0) * 4)) = param_5;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_80 + (uint32_t)(1) * 4)) = param_6;
    if ((param_2 & 4) != 0) {
      sceSifWriteBackDCache();
      iVar2 = 0x10;
      goto LAB_00287378;
    }
  }
  iVar2 = iVar4 << 4;
LAB_00287378:
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)auStack_80 + iVar2))) = puVar3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)auStack_80 + iVar2 + 4))) = (*(uint32_t*)rtguest::G(0x1776ea0));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)auStack_80 + iVar2 + 8))) = (int)param_4;
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint32_t))) = param_1;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)puVar3)) = (uint32_t)param_4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)auStack_80 + iVar2 + 0xc))) = 0x44;
  sceSifWriteBackDCache(param_3,param_4);
  if ((param_2 & 1) == 0) {
    uVar1 = sceSifSetDma(auStack_80,iVar4 + 1);
  }
  else {
    uVar1 = isceSifSetDma(auStack_80);
  }
  return uVar1;
}


RT_GUEST_FN(0x002872d8, guest::_sceSifSendCmd)

}  // namespace guest
