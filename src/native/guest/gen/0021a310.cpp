// guest:0x0021a310 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_controls_car_update(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  byte bVar1;
  short sVar2;
  undefined1 uVar3;
  short sVar4;
  undefined8 uVar5;
  short sVar6;
  int iVar7;
  int iVar8;
  long lVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  iVar16 = 0x59;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_60)), 0, 0x10));
  iVar13 = (int)param_2;
  if ((*(int64_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1e0))) == 0) {
    iVar16 = 0;
  }
  iVar14 = (*(uint32_t*)rtguest::G((uint32_t)param_5 + (uint32_t)(1) * sizeof(uint32_t)));
  uVar3 = rtCall((*(uint32_t*)rtguest::G(0x3356d8)), (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1ff))), (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))), param_4, iVar13 + 0x22c, (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d0))));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1ff))) = uVar3;
  iVar15 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t)));
  if (iVar15 < 0) {
    iVar15 = -iVar15;
  }
  if (iVar15 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8)))) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8))) = iVar15;
  }
  lVar9 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1ce)));
  iVar7 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1ce)));
  if ((param_4 & 0x8000) == 0) {
    if ((param_4 & 0x2000) == 0) {
      if (iVar15 != 0) {
        if (lVar9 < 4) {
          if (lVar9 < 1) {
            if (lVar9 < -3) {
              lVar9 = (long)(iVar7 + 4);
            }
            else if (lVar9 < 0) {
              lVar9 = 0;
            }
          }
          else {
            lVar9 = 0;
          }
        }
        else {
          lVar9 = (long)(iVar7 + -4);
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8))) = iVar15;
      }
    }
    else {
      if (lVar9 < 0) {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8))) = iVar15;
        goto LAB_0021a3f0;
      }
      iVar7 = iVar7 + 1;
      if (iVar7 < 0x21) {
        iVar8 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x242)));
        goto LAB_0021a454;
      }
      lVar9 = 0x20;
    }
LAB_0021a450:
    iVar7 = (int)lVar9;
    iVar8 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x242)));
  }
  else {
    if (0 < lVar9) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8))) = iVar15;
LAB_0021a3f0:
      lVar9 = (long)-iVar7;
      goto LAB_0021a450;
    }
    iVar7 = iVar7 + -1;
    if (iVar7 < -0x20) {
      lVar9 = -0x20;
      goto LAB_0021a450;
    }
    iVar8 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x242)));
  }
LAB_0021a454:
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1ce))) = (short)iVar7;
  iVar8 = iVar8 * iVar7;
  if (iVar8 < 0) {
    iVar8 = iVar8 + 0x1f;
  }
  iVar8 = iVar8 >> 5;
  sVar4 = 0;
  if (iVar8 != 0) {
    if (iVar8 == 0) {
      trap(7);
    }
    iVar7 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d8))) * 0x28be) / iVar8;
    if (iVar15 < 0) {
      iVar7 = -iVar7;
    }
    if ((0 < iVar8) && (iVar7 < 0x18000)) {
      iVar7 = 0x18000;
    }
    if ((iVar8 < 0) && (-0x18000 < iVar7)) {
      iVar7 = -0x18000;
    }
    if (iVar7 == 0) {
      trap(7);
    }
    sVar4 = (short)(0x1b4c14b6 / iVar7);
  }
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1cc))) = sVar4;
  if (sVar4 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_5)) = 0;
  }
  if ((param_4 & 2) == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe))) = 0;
    iVar15 = 0;
  }
  else {
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe))) < 0x20) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe))) + 1;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe)));
    }
    else {
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1fe)));
    }
    iVar15 = (int)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x200))) + (uint32_t)bVar1 + -1))) * 10000) >> 5;
  }
  iVar8 = iVar13 + 0xc;
  iVar7 = iVar16 * (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x19c))) & 7) * 2 + iVar8 + 0x210)));
  if (iVar7 < 0) {
    iVar7 = iVar7 + 0xff;
  }
  uVar5 = AdvanceVehicleMotion
                    (auStack_60,param_2,(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)param_3)),iVar7 >> 8,iVar15,param_4,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x28))) & 4);
  UpdateCarTireSound(param_1,param_2,uVar5,param_4);
  iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x218)));
  iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1b8)));
  iVar12 = 0;
  if (iVar10 != 0) {
    iVar12 = iVar15;
  }
  iVar14 = iVar14 * (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x19c))) & 7) * 2 + iVar8 + 0x210)));
  if (iVar14 < 0) {
    iVar14 = iVar14 + 0xff;
  }
  iVar14 = (iVar14 >> 8) * iVar7;
  iVar11 = 0;
  if (iVar12 == 0) {
    iVar11 = iVar10;
  }
  if ((iVar14 != 0) && (iVar11 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))), iVar12 != 0)) {
    iVar11 = ((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t))) - iVar10) * iVar7;
    if (-1 < iVar14) {
      if (iVar11 < -iVar14) {
        iVar11 = -iVar14;
      }
      if (iVar14 < iVar11) {
        iVar11 = iVar14;
      }
    }
    iVar12 = iVar12 / iVar7;
    if (iVar7 == 0) {
      trap(7);
    }
    iVar10 = iVar11 / iVar7 + iVar10;
    if (iVar10 < 0) {
      iVar11 = iVar10;
      if (iVar10 <= -iVar12) {
        iVar11 = iVar10 + iVar12;
        goto LAB_0021a670;
      }
    }
    else {
      iVar11 = iVar12;
      if (iVar10 < iVar12) {
        iVar11 = iVar10;
      }
    }
    iVar11 = iVar10 - iVar11;
  }
LAB_0021a670:
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1b8))) = iVar11;
  if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1dc))) == 0) && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1e0))) == 0)) {
    sVar6 = 0;
    sVar4 = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x213))) == '\0') goto LAB_0021a6cc;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x184))) + 8))) & 0x100) != 0) goto code_r0x0021a688;
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d6)));
  }
  else {
code_r0x0021a688:
    sVar4 = (short)((iVar11 * (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1cc)))) / 0xab85);
LAB_0021a6cc:
    sVar6 = sVar4;
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d6)));
  }
  iVar14 = (int)sVar6;
  if (sVar4 == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d4))) + sVar6;
    iVar7 = (iVar14 * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1b8)))) / 0x28be;
    goto LAB_0021a7a4;
  }
  if ((sVar4 < 1) || (iVar14 < 1)) {
    if (sVar4 < 0) {
      if (iVar14 < 0) {
        sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d2)));
        goto LAB_0021a714;
      }
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d2)));
    }
    else {
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d2)));
    }
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d2)));
    if (sVar2 != 0) {
      iVar7 = ((int)sVar2 * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1b8)))) / 0x28be;
      goto LAB_0021a7a4;
    }
  }
  else {
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d2)));
  }
LAB_0021a714:
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1d4))) + sVar6;
  iVar7 = ((iVar14 + sVar4) * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x1b8)))) / 0x28be;
LAB_0021a7a4:
  TransformVecByF15Matrix(iVar13 + 0xf0,param_2,auStack_60);
  iVar16 = iVar16 * (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x19c))) & 7) * 2 + iVar8 + 0x210)));
  if (iVar16 < 0) {
    iVar16 = iVar16 + 0xff;
  }
  UpdateSteeringDamping(param_2,iVar14,iVar16 >> 8,iVar15,iVar7);
  return 0;
}


RT_GUEST_FN(0x0021a310, guest::gs_controls_car_update)

}  // namespace guest
