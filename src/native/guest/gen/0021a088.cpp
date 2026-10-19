// guest:0x0021a088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateSteeringDamping(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  bool bVar1;
  int iVar2;
  int iVar3;
  long lVar4;
  long lVar5;
  uint uVar6;
  short sVar7;
  
  sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
  if ((uint32_t)(sVar7 + 0xfffU) < 0x1fff) {
    if (param_3 < 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = 0;
    }
    else if ((param_5 < -param_3) || (param_3 < param_5)) {
      if (param_5 < 0) {
        iVar3 = (param_5 - param_3) / 3;
        if (iVar3 < -0x200) {
          iVar3 = -0x200;
        }
        if (iVar3 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) < 9) {
LAB_0021a150:
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = (short)iVar3;
        }
        else {
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) + -8;
        }
      }
      else {
        iVar3 = (param_5 + param_3) / 3;
        if (0x200 < iVar3) {
          iVar3 = 0x200;
        }
        if (iVar3 - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) < 9) goto LAB_0021a150;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) + 8;
      }
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = 0;
    }
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1b8)));
  if (iVar3 == 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = 0;
  }
  if (sVar7 < 0) {
LAB_0021a188:
    uVar6 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2)));
    if (param_2 < 1) goto LAB_0021a190;
LAB_0021a1a0:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1f8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1f8))) | 0x20;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6))) = sVar7 + (short)((int)((int)(short)uVar6 + (uVar6 >> 0xf)) >> 1);
  }
  else {
    if (param_2 < 0) {
      if (sVar7 < 1) goto LAB_0021a188;
      uVar6 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2)));
      goto LAB_0021a1a0;
    }
    uVar6 = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2)));
LAB_0021a190:
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6))) = sVar7 + (short)uVar6;
  }
  sVar7 = (short)uVar6;
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4))) + sVar7;
  if (param_4 < 5000) {
    iVar2 = (int)sVar7;
    if (iVar2 < 0) {
      iVar2 = iVar2 + 7;
    }
    uVar6 = iVar2 >> 3;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d2))) = (short)uVar6;
LAB_0021a260:
    iVar2 = uVar6 << 0x10;
  }
  else if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6))) < 0) {
LAB_0021a234:
    iVar2 = uVar6 << 0x10;
    if ((int)((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1cc))) << 0x10) < 0) {
LAB_0021a23c:
      iVar2 = (int)sVar7;
      if (iVar2 < 0) {
        iVar2 = iVar2 + 7;
      }
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4))) + (short)(iVar2 >> 3);
      goto LAB_0021a260;
    }
  }
  else {
    iVar2 = uVar6 << 0x10;
    if (0 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1cc)))) goto LAB_0021a23c;
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6))) < 1) goto LAB_0021a234;
  }
  if (iVar2 != 0) {
    return 0;
  }
  lVar4 = 0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1bc))) == 0) {
    if (iVar3 != 0) {
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
      goto LAB_0021a2ec;
    }
  }
  else if (iVar3 != 0) {
    if (iVar3 == 0) {
      trap(7);
    }
    lVar4 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
    iVar3 = 0x1ec000 / iVar3;
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
    lVar5 = (long)iVar3;
    if ((long)iVar3 < 0) {
      lVar5 = (long)-iVar3;
    }
    if (lVar4 < 0) {
      lVar5 = (long)-(int)lVar5;
      bVar1 = lVar5 < lVar4;
    }
    else {
      bVar1 = lVar4 < lVar5;
    }
    if (!bVar1) {
      lVar4 = lVar5;
    }
    goto LAB_0021a2ec;
  }
  sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
  lVar4 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6)));
LAB_0021a2ec:
  if (lVar4 < -0x400) {
    lVar4 = -0x400;
  }
  if (0x400 < lVar4) {
    lVar4 = 0x400;
  }
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d6))) = sVar7 - (short)lVar4;
  return 0;
}


RT_GUEST_FN(0x0021a088, guest::UpdateSteeringDamping)

}  // namespace guest
