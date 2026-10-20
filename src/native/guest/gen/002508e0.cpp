// guest:0x002508e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CalculateAiSteeringAndThrottle(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  byte bVar1;
  byte bVar2;
  uint uVar3;
  byte bVar4;
  uint uVar5;
uint32_t pcVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
uint32_t pbVar11;
  int iVar12;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x250))) == 0) {
    return 0;
  }
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x210))) == 0) {
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x254)));
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) == -1) {
      return 0;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) == '\"') {
      return 0;
    }
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x254)));
  }
  uVar3 = ComputeWaypointTargetAngle
                    ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x250))) + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_3 * 8 + iVar8 + 1))) * 0x18,
                     param_2 + 0x90,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x194))) & 3);
  iVar8 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1b8))) * 0xd8) / 0x28000;
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
  if (param_4 == 0) {
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
LAB_00250c1c:
    uVar5 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
  }
  else {
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + param_3))) == 0) ||
       (bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_3))), ((long)(char)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_3))) & 0xc0U) == 0)
       ) {
LAB_00250aa4:
      pbVar11 = (uint32_t)(iVar12 + param_3);
    }
    else {
      uVar5 = bVar1 & 0x3f;
      iVar10 = 0;
      if ((bVar1 & 0x3f) != 0) {
        if (uVar5 - 3 < 0x3e) {
          iVar10 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + param_3))) - (uVar5 - 2);
        }
        else {
          iVar10 = 0;
        }
      }
      pbVar11 = (uint32_t)(iVar12 + param_3);
      if (iVar10 < 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_3))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_3))) & 0xbf;
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
        pbVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600))) + (uint32_t)bVar1);
        if (1 < (*(int8_t*)rtguest::G((uint32_t)pbVar11))) {
          iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
          bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + (uint32_t)bVar1)));
          uVar5 = bVar2 & 0x3f;
          iVar9 = 0;
          if (((bVar2 & 0x3f) != 0) && (uVar5 - 2 < 0x3e)) {
            iVar9 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar11)) - (uVar5 - 1);
          }
          if (iVar9 < 1) {
            pbVar11 = (uint32_t)(iVar12 + param_3);
            goto LAB_00250aa8;
          }
          pcVar6 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600))) + (uint32_t)bVar1);
          (*(int8_t*)rtguest::G((uint32_t)pcVar6)) = (*(int8_t*)rtguest::G((uint32_t)pcVar6)) + -1;
        }
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
        goto LAB_00250aa4;
      }
    }
LAB_00250aa8:
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar11));
    if (((long)(char)(*(int8_t*)rtguest::G((uint32_t)pbVar11)) & 0x80U) == 0) {
      if ((bVar1 & 0x40) == 0) {
        iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
      }
      else {
        (*(int8_t*)rtguest::G((uint32_t)pbVar11)) = bVar1 & 0x3f;
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
        uVar5 = (uint32_t)bVar1;
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
        bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + (uint32_t)bVar1)));
        uVar7 = bVar2 & 0x3f;
        if ((bVar2 & 0x3f) == 0) {
          iVar10 = 0;
          iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
        }
        else {
          iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
          if (uVar7 < 0x3e) {
            iVar10 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + (uint32_t)bVar1))) - (uVar7 + 1);
          }
          else {
            iVar10 = 0;
          }
        }
        pbVar11 = (uint32_t)(iVar9 + (uint32_t)bVar1);
        if (-1 < iVar10) goto LAB_00250c24;
        bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + (uint32_t)bVar1)));
        bVar4 = bVar2 & 0x3f;
        if ((bVar2 & 0x3f) == 0) goto LAB_00250c20;
        pbVar11 = (uint32_t)(iVar9 + (uint32_t)bVar1);
        if (0x3d < bVar4) goto LAB_00250c24;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + (uint32_t)bVar1))) = bVar4 + 1;
        iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
      }
      goto LAB_00250c1c;
    }
    (*(int8_t*)rtguest::G((uint32_t)pbVar11)) = bVar1 & 0x3f;
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a)));
    uVar5 = (uint32_t)bVar1;
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
    pbVar11 = (uint32_t)(iVar9 + (uint32_t)bVar1);
    if ((*(int8_t*)rtguest::G((uint32_t)pbVar11)) == 0) {
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
    }
    else {
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
      bVar2 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + (uint32_t)bVar1)));
      uVar7 = bVar2 & 0x3f;
      iVar10 = 0;
      if (((bVar2 & 0x3f) != 0) && (uVar7 - 2 < 0x3e)) {
        iVar10 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar11)) - (uVar7 - 1);
      }
      if (iVar10 < 1) {
        pbVar11 = (uint32_t)(iVar9 + (uint32_t)bVar1);
        goto LAB_00250c24;
      }
    }
    uVar7 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + uVar5))) & 0x3f;
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + uVar5))) & 0x3f) != 0) {
      pbVar11 = (uint32_t)(iVar9 + (uint32_t)bVar1);
      if (0x3d < uVar7 - 2) goto LAB_00250c24;
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + uVar5))) = (char)uVar7 + -1;
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 600)));
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c)));
      goto LAB_00250c1c;
    }
  }
LAB_00250c20:
  pbVar11 = (uint32_t)(iVar9 + uVar5);
LAB_00250c24:
  iVar9 = iVar8 - ((*(int8_t*)rtguest::G((uint32_t)pbVar11)) & 0x7f);
  uVar7 = 1;
  if (((*(int8_t*)rtguest::G((uint32_t)pbVar11)) & 0x7f) != 0) {
    uVar7 = 2;
    if (iVar9 < 2) {
      uVar7 = (uint32_t)(iVar9 < 1);
    }
  }
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x246))) != '\0') {
    iVar9 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1b8))) * 0xd8) / 0x14000 - (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x246)));
    if (0 < iVar9) {
      uVar7 = 0;
    }
    if (2 < iVar9) {
      uVar7 = 2;
    }
  }
  iVar9 = (int)(((uVar3 & 0xffff) - (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4)))) * 0x10000) >> 0x10;
  if (iVar9 < 1) {
    if ((int)(((uVar3 >> 0x10) - (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4)))) * 0x10000) >> 0x10 < 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + uVar5))) = 0x80;
      if (iVar9 < -0x4000) {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0xc) == 4) {
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))) + -0x100;
        }
      }
      else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19a))) != '\0') {
        uVar7 = uVar7 | 0x8000;
      }
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + uVar5))) = 0x40;
      if (uVar7 == 1) {
        uVar7 = 9;
      }
    }
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + uVar5))) = 0x80;
    if (iVar9 < 0x4001) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19a))) != '\0') {
        uVar7 = uVar7 | 0x2000;
      }
    }
    else if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0xc) == 4) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d4))) + 0x100;
    }
  }
  if (iVar8 - 2U < 0x3d) {
    pbVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x25c))) + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24a))));
    (*(int8_t*)rtguest::G((uint32_t)pbVar11)) = (*(int8_t*)rtguest::G((uint32_t)pbVar11)) | (uint32_t)iVar8;
  }
  return uVar7;
}


RT_GUEST_FN(0x002508e0, guest::CalculateAiSteeringAndThrottle)

}  // namespace guest
