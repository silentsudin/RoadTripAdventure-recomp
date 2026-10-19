// guest:0x00281118 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __swsetup_r(uint32_t param_1, ...) {
uint32_t puVar1;
  int iVar2;
uint32_t puVar3;
  ushort uVar4;
uint32_t piVar5;
  
  puVar3 = (*(uint32_t*)rtguest::G(0x002e8cc0));
  piVar5 = (uint32_t)param_1;
  puVar1 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0x15) * sizeof(int32_t)));
  if (puVar1 == (uint32_t)0x0) {
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0x15) * sizeof(int32_t))) = (int)(*(uint32_t*)rtguest::G(0x002e8cc0));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x38)));
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x38)));
    puVar3 = puVar1;
  }
  if (iVar2 == 0) {
    __sinit(puVar3);
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3)));
  }
  else {
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3)));
  }
  if ((uVar4 & 8) == 0) {
    if ((uVar4 & 0x10) == 0) {
      return 0xffffffff;
    }
    if ((uVar4 & 4) == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(4) * sizeof(int32_t)));
    }
    else {
      if ((uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0xc) * sizeof(int32_t))) != (uint32_t)0x0) {
        if ((uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0xc) * sizeof(int32_t))) == piVar5 + 0x10) {
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0xc) * sizeof(int32_t))) = 0;
        }
        else {
          _free_r((*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0x15) * sizeof(int32_t))));
          uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3)));
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(0xc) * sizeof(int32_t))) = 0;
        }
      }
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(4) * sizeof(int32_t)));
      uVar4 = uVar4 & 0xffdb;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3))) = uVar4;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = iVar2;
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3))) = uVar4 | 8;
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(4) * sizeof(int32_t)));
  }
  if (iVar2 == 0) {
    __smakebuf(param_1);
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3)));
  }
  else {
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(piVar5 + 3)));
  }
  if ((uVar4 & 1) == 0) {
    iVar2 = 0;
    if ((uVar4 & 2) == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(5) * sizeof(int32_t)));
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(2) * sizeof(int32_t))) = iVar2;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(2) * sizeof(int32_t))) = 0;
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(6) * sizeof(int32_t))) = -(*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(5) * sizeof(int32_t)));
  }
  return 0;
}


RT_GUEST_FN(0x00281118, guest::__swsetup_r)

}  // namespace guest
