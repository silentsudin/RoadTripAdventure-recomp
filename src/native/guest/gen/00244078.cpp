// guest:0x00244078 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LayoutRaceRowQuads(uint32_t param_1, uint32_t param_2, ...) {
uint32_t puVar1;
uint32_t puVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
uint32_t pbVar9;
uint32_t pbVar10;
  
  iVar6 = 0;
  iVar8 = 0;
  iVar7 = param_1 + 0x20;
  gifSetTexRegs(param_1,0x2003df66a1411d43,5);
  pbVar10 = 0x2be1d8;
  pbVar9 = 0x2be158;
  do {
    if (iVar6 < 10) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\x01') {
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar9));
      }
      else {
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar10));
      }
    }
    else {
      uVar5 = 0x19;
      if (iVar6 != 0xc) {
        uVar5 = 0x1b;
        if (iVar6 != 0xd) {
          uVar5 = iVar6 + 0x10;
        }
      }
    }
    gifTexQuadTag(iVar7);
    uVar3 = 0;
    puVar1 = (uint32_t)(param_1 + iVar8);
    do {
      puVar2 = puVar1 + 0xc;
      if ((uVar3 & 1) == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0x3f400000;
      }
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0x3f800000;
      uVar4 = uVar3 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0x17) * sizeof(uint32_t))) = 0xff0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(0xd) * sizeof(uint32_t))) = (float)(int)((uVar5 + (int)uVar3 / 2) * 0x1c) * 0.0009765625;
      uVar3 = uVar4;
      puVar1 = puVar2;
    } while ((int)uVar4 < 4);
    iVar6 = iVar6 + 1;
    iVar7 = iVar7 + 0xd0;
    iVar8 = iVar8 + 0x50;
    param_1 = param_1 + 0x80;
    pbVar10 = pbVar10 + 0x10;
    pbVar9 = pbVar9 + 0x10;
  } while (iVar6 < 0x10);
  return 0;
}


RT_GUEST_FN(0x00244078, guest::LayoutRaceRowQuads)

}  // namespace guest
