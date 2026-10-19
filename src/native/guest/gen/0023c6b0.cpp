// guest:0x0023c6b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FindIndexByMaskedByte(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  byte bVar1;
  bool bVar2;
uint32_t pbVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
  undefined1 uVar7;
  uint uVar8;
  
  if (param_2 == 0) {
    trap(7);
  }
  iVar5 = 0;
  uVar8 = param_3 % param_2 + param_1;
  do {
    uVar7 = (uint32_t)uVar8;
    iVar6 = 0;
    if ((*(int32_t*)rtguest::G(0x177fd88)) < 1) {
LAB_0023c71c:
      bVar2 = false;
    }
    else {
      pbVar3 = 0x0177fd08;
      bVar1 = (*(uint8_t*)rtguest::G(0x177fd08));
      while( true ) {
        pbVar3 = pbVar3 + 1;
        bVar2 = true;
        if ((bVar1 & 0x7f) == (uVar8 & 0xff)) break;
        iVar6 = iVar6 + 1;
        if ((*(int32_t*)rtguest::G(0x177fd88)) <= iVar6) goto LAB_0023c71c;
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
      }
    }
    if (!bVar2) goto LAB_0023c750;
    iVar5 = iVar5 + 1;
    if (param_2 == 0) {
      trap(7);
    }
    uVar8 = (param_3 + iVar5) % param_2 + param_1;
  } while (iVar5 != param_2);
  uVar7 = (uint32_t)uVar8;
LAB_0023c750:
  if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
    puVar4 = 0x0177fd08;
    iVar5 = 0;
    do {
      iVar5 = iVar5 + 1;
      (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint8_t)));
      puVar4 = puVar4 + 1;
      (*(uint32_t*)rtguest::G(0x177fd87)) = uVar7;
    } while (iVar5 < 0x7f);
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = uVar7;
    (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
  }
}


RT_GUEST_FN(0x0023c6b0, guest::FindIndexByMaskedByte)

}  // namespace guest
