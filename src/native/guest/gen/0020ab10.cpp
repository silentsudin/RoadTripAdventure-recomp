// guest:0x0020ab10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateControllerInput(uint32_t param_1, ...) {
    uint32_t uStack_70 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
uint32_t auStack_60 = rtguest::allocFrame(2);
    uint32_t bStack_5e = rtguest::allocFrame(1);
    uint32_t bStack_5d = rtguest::allocFrame(1);
uint32_t abStack_5c = rtguest::allocFrame(28);
  byte bVar1;
  char cVar2;
  uint uVar3;
  long lVar4;
  uint uVar5;
uint32_t pcVar6;
uint32_t puVar7;
uint32_t pbVar8;
  int iVar9;
uint32_t puVar10;
  int iVar11;
uint32_t puVar12;
  int iVar13;
uint32_t pbVar14;
  int iVar15;
  
  pbVar14 = (uint32_t)uStack_70;
  iVar11 = (int)param_1;
  puVar12 = (uint32_t)(iVar11 + 0x3c);
  iVar13 = 0;
  iVar15 = 0;
  do {
    lVar4 = ReadPadState(param_1,iVar13,auStack_60);
    if (lVar4 == 0) {
      (*(int8_t*)rtguest::G((uint32_t)pbVar14 + (uint32_t)(0) * sizeof(int8_t))) = 0;
      (*(int8_t*)rtguest::G((uint32_t)pbVar14 + (uint32_t)(1) * sizeof(int8_t))) = 0;
LAB_0020abdc:
      (*(uint32_t*)rtguest::G((uint32_t)puVar12)) = 0;
    }
    else {
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0xc + iVar13)));
      (*(int8_t*)rtguest::G((uint32_t)pbVar14 + (uint32_t)(1) * sizeof(int8_t))) = ~(*(int8_t*)rtguest::G((uint32_t)bStack_5e));
      (*(int8_t*)rtguest::G((uint32_t)pbVar14)) = ~(*(int8_t*)rtguest::G((uint32_t)bStack_5d));
      if ((bVar1 & 2) == 0) goto LAB_0020abdc;
      iVar9 = 0;
      puVar7 = (uint32_t)(iVar15 + iVar11 + 0xc + 0x30);
      pbVar8 = abStack_5c;
      do {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
        pbVar8 = pbVar8 + 1;
        iVar9 = iVar9 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar7)) = (*(uint8_t*)rtguest::G((uint32_t)0x29ad80 + (uint32_t)(bVar1 >> 4) * 1));
        puVar7 = puVar7 + 1;
      } while (iVar9 < 4);
    }
    iVar13 = iVar13 + 1;
    puVar12 = puVar12 + 0x4;  // T* element step
    pbVar14 = pbVar14 + 2;
    iVar15 = iVar15 + 4;
    if (1 < iVar13) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x2c)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x2c))) = (*(uint32_t*)rtguest::G((uint32_t)uStack_70));
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) & ~uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x38))) = ~(*(uint32_t*)rtguest::G((uint32_t)uStack_70)) & uVar3;
      puVar10 = 0x002ea44c;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x34))) = uVar5;
      iVar13 = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x30))) = uVar5;
      pcVar6 = (uint32_t)((int)0x00335694 + 3);
      do {
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
        puVar10 = puVar10 + -1;
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x2c))) & uVar3) == 0) {
          (*(int8_t*)rtguest::G((uint32_t)pcVar6)) = '\0';
        }
        else {
          cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar6));
          if (cVar2 == '\x15') {
            (*(int8_t*)rtguest::G((uint32_t)pcVar6)) = '\x12';
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x34))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x34))) | uVar3;
            cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar6));
          }
          (*(int8_t*)rtguest::G((uint32_t)pcVar6)) = cVar2 + '\x01';
        }
        iVar13 = iVar13 + -1;
        pcVar6 = pcVar6 + -1;
      } while (iVar13 != -1);
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x0020ab10, guest::UpdateControllerInput)

}  // namespace guest
