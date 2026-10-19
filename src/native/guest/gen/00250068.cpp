// guest:0x00250068 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateVehicleWaypointProgress(uint32_t param_1, uint32_t param_2, ...) {
  byte bVar1;
  int iVar2;
  byte bVar3;
  short sVar4;
uint32_t pfVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  char cVar11;
uint32_t pbVar12;
  int iVar13;
uint32_t pfVar14;
  char cVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  float fVar21;
uint32_t auStack_c0 = rtguest::allocFrame(32);
uint32_t auStack_a0 = rtguest::allocFrame(64);
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x250)));
  if (iVar2 == 0) {
    return 0;
  }
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
  pfVar14 = (uint32_t)(param_2 + 0x90);
  iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x254)));
  pbVar12 = (uint32_t)(iVar13 + (uint32_t)bVar1 * 8);
  uVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12));
  pfVar5 = (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12)) * 0x18 + iVar2);
  if (((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(1) * sizeof(float)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float)))) -
      ((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float)))) < 0.0) {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(2) * sizeof(int8_t)));
    while( true ) {
      if (bVar1 == (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(3) * sizeof(int8_t)))) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = bVar1;
      }
      else {
        iVar6 = uVar8 * 0x18 + iVar2;
        iVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t))) * 0x18 + iVar2;
        fVar17 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)));
        fVar16 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x14)));
        if ((fVar16 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - fVar17) -
            (fVar17 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10)))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - fVar16) < 0.0) {
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = bVar1;
        }
        else {
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(3) * sizeof(int8_t)));
        }
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
      pbVar12 = (uint32_t)(iVar13 + (uint32_t)bVar1 * 8);
      uVar8 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12));
      pfVar5 = (uint32_t)(uVar8 * 0x18 + iVar2);
      if (0.0 <= ((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(1) * sizeof(float)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float)))) -
                 ((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float))))) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(2) * sizeof(int8_t)));
    }
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(6) * sizeof(int8_t)));
  }
  else {
    while( true ) {
      pfVar5 = (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t))) * 0x18 + iVar2);
      if (((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(1) * sizeof(float)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float)))) -
          ((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float)))) < 0.0) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(4) * sizeof(int8_t)));
      if (bVar1 == (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(5) * sizeof(int8_t)))) {
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = bVar1;
      }
      else {
        iVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12)) * 0x18 + iVar2;
        if (((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(5) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(4) * sizeof(float)))) -
            ((*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(4) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10)))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(5) * sizeof(float)))) < 0.0)
        {
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = bVar1;
        }
        else {
          (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))) = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(5) * sizeof(int8_t)));
        }
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
      pbVar12 = (uint32_t)(iVar13 + (uint32_t)bVar1 * 8);
    }
    bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(6) * sizeof(int8_t)));
  }
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24b))) = bVar3;
  cVar15 = '\x02';
  if ((*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(4) * sizeof(int8_t))) != (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(5) * sizeof(int8_t)))) {
    iVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12)) * 0x18 + iVar2;
    iVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t))) * 0x18 + iVar2;
    fVar16 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10)));
    fVar17 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)));
    cVar15 = 0.0 <= (fVar17 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x14)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - fVar16) -
                    (fVar16 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - fVar17);
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb))) == '\0') {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t)));
  }
  else {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 3) != 0) {
      cVar11 = '\x02';
      if ((cVar15 == '\x02') && ((*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(2) * sizeof(int8_t))) != (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(3) * sizeof(int8_t))))) {
        iVar7 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12)) * 0x18 + iVar2;
        iVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t))) * 0x18 + iVar2;
        fVar16 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10)));
        fVar17 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x14)));
        cVar11 = 0.0 <= (fVar17 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x14)))) * ((*(float*)rtguest::G((uint32_t)pfVar14)) - fVar16) -
                        (fVar16 - (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)))) * ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98))) - fVar17);
      }
      uVar9 = 0x3344b0;
      if (cVar15 != '\x02') {
        if (cVar15 == '\0') {
          uVar9 = 0x3344a8;
        }
        else {
          uVar9 = 0x3344a0;
        }
      }
      uVar10 = 0x3344b0;
      if (cVar11 != '\x02') {
        if (cVar11 == '\0') {
          uVar10 = 0x3344a8;
        }
        else {
          uVar10 = 0x3344a0;
        }
      }
      ::sprintf((char *)rtguest::G((uint32_t)(auStack_c0)), (char *)rtguest::G((uint32_t)(0x328008)), bVar1, uVar9, uVar10);
      uiDebugText8(0x10,0x98,auStack_c0,0xf);
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(1) * sizeof(int8_t)));
  }
  pfVar5 = (uint32_t)((uint32_t)bVar1 * 0x18 + iVar2);
  fVar16 = (float)atan2f((*(float*)rtguest::G((uint32_t)pfVar5)) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float))),(*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(1) * sizeof(float))) - (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float))));
  uVar8 = (int)(fVar16 * (*(float*)rtguest::G(0x332fa8))) * 0x10000 + -0x40000000 >> 0x10;
  iVar6 = (int)((uVar8 - (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4)))) * 0x10000) >> 0x10;
  if (iVar6 < -0x5000) {
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x248)));
  }
  else {
    if (iVar6 < 0x5001) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x248))) = 0;
      goto LAB_0025058c;
    }
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x248)));
  }
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x248))) = sVar4 + 1;
LAB_0025058c:
  if (cVar15 == '\0') {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(4) * sizeof(int8_t)));
  }
  else {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar12 + (uint32_t)(5) * sizeof(int8_t)));
  }
  iVar13 = iVar13 + (uint32_t)bVar1 * 8;
  fVar21 = (float)(int)uVar8 / (*(uint32_t*)rtguest::G(0x332fac));
  fVar18 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x98)));
  pfVar5 = (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 1))) * 0x18 + iVar2);
  fVar16 = (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(3) * sizeof(float)));
  fVar20 = (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(1) * sizeof(float)));
  fVar19 = ((*(float*)rtguest::G((uint32_t)pfVar5)) + (*(float*)rtguest::G((uint32_t)pfVar5 + (uint32_t)(2) * sizeof(float)))) * 0.5 - (*(float*)rtguest::G((uint32_t)pfVar14));
  fVar17 = (float)cosf(fVar21);
  fVar21 = (float)sinf(fVar21);
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24c))) = ((fVar20 + fVar16) * 0.5 - fVar18) * fVar17 + fVar19 * fVar21;
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb))) != '\0') && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 3) == 2)) {
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_a0)), (char *)rtguest::G((uint32_t)(0x328018)), (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 1))), uVar8 & 0xffff);
    uiDebugText8(0x10,0xa0,auStack_a0,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_a0)), (char *)rtguest::G((uint32_t)(0x328030)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))));
    uiDebugText8(0x10,0xa8,auStack_a0,0xf);
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_a0)), (char *)rtguest::G((uint32_t)(0x328040)), (int)((*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24c))) * 100.0));
    uiDebugText8(0x10,0xb0,auStack_a0,0xf);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x248))) != 0) {
      uiDebugText8(300,0x60,0x3344b8,0xf);
    }
  }
}


RT_GUEST_FN(0x00250068, guest::UpdateVehicleWaypointProgress)

}  // namespace guest
