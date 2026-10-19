// guest:0x002058a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiMenuAddItem(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  byte bVar1;
  short sVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
uint32_t psVar7;
  byte bVar8;
uint32_t pbVar9;
  long lVar10;
  byte bVar11;
  
  psVar7 = (uint32_t)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf))) == '\x0f') {
    fatal(0x2ea070);
  }
  bVar11 = param_3 & 0x80;
  bVar8 = param_3;
  if ((bVar11 != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf))) == '\0')) {
    bVar8 = param_3 & 0x7f;
    bVar11 = 0;
  }
  param_3 = param_3 & 0x10;
  if (param_3 == 0) {
    uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
  }
  else if (bVar11 == 0) {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xb))) + '\x01';
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xb))) = cVar3;
    if (cVar3 == '\x01') {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + (uint32_t)(5) * sizeof(int16_t)))) = 0;
      if (uVar6 != 0) {
        pbVar9 = (uint32_t)(psVar7 + (uint32_t)(0x28) * sizeof(int16_t));
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
        uVar5 = uVar6;
        while( true ) {
          pbVar9 = pbVar9 + 1;
          if ((bVar1 & 0x80) == 0) {
            (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + (uint32_t)(5) * sizeof(int16_t)))) = (char)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(5) * sizeof(int16_t))) + '\x01';
          }
          uVar5 = uVar5 - 1;
          if (uVar5 == 0) break;
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
        }
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xd))) = 0;
      (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(4) * sizeof(int16_t))) = (short)((int)((int)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(2) * sizeof(int16_t))) + (uint32_t)((uint32_t)(uint16_t)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(2) * sizeof(int16_t))) >> 0xf)) >> 1);
    }
    else {
      uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
    }
  }
  else {
    uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + (uVar6 * 2 + 10) * sizeof(int16_t)))) = param_2;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf))) + 0x50))) = bVar8;
  bVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
  pbVar9 = (uint32_t)((int)psVar7 + bVar8 + 0x50);
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
  lVar10 = 0;
  if ((bVar1 & 0x40) == 0) {
    lVar10 = uiTextWidth((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + ((uint32_t)bVar8 * 2 + 10) * sizeof(int16_t)))));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar9));
  }
  if ((bVar1 & 0x80) != 0) {
    iVar4 = MeasureTextWidth(param_1,bVar8 - 1);
    lVar10 = (long)((int)lVar10 + iVar4);
  }
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(2) * sizeof(int16_t)));
  if (param_3 != 0) {
    lVar10 = (long)((int)lVar10 + 0x14);
  }
  if (sVar2 < lVar10) {
    (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(2) * sizeof(int16_t))) = (short)lVar10;
    sVar2 = (short)(((int)lVar10 - (int)sVar2) / 2);
    (*(int16_t*)rtguest::G((uint32_t)psVar7)) = (*(int16_t*)rtguest::G((uint32_t)psVar7)) - sVar2;
    (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(4) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(4) * sizeof(int16_t))) + sVar2;
  }
  if (param_3 == 0) {
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
  }
  else {
    iVar4 = ((int)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(2) * sizeof(int16_t))) - (int)lVar10) / 2;
    if ((long)iVar4 < (long)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(4) * sizeof(int16_t)))) {
      (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(4) * sizeof(int16_t))) = (short)iVar4;
    }
    cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf)));
  }
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 0xf))) = cVar3 + '\x01';
  if (bVar11 == 0) {
    (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(1) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(1) * sizeof(int16_t))) + -6;
    (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(3) * sizeof(int16_t))) = (*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(3) * sizeof(int16_t))) + 0xc;
  }
  return 0;
}


RT_GUEST_FN(0x002058a8, guest::uiMenuAddItem)

}  // namespace guest
