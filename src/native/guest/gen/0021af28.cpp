// guest:0x0021af28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateWheelSuspension(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t aiStack_70 = rtguest::allocFrame(16);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
uint32_t pfVar5;
uint32_t puVar6;
uint32_t piVar7;
uint32_t piVar8;
uint32_t piVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  float fVar13;
  
  iVar2 = 0xa3d7;
  uVar11 = 0;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_5 + (uint32_t)(2) * sizeof(int32_t)));
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x184))) + 8))) & 0x400) == 0) {
    iVar2 = 0x51eb;
  }
  if (iVar3 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -iVar3;
    if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) < 0) {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) + 3;
    }
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) >> 2;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -iVar3;
    if ((*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) < 0) {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) + 7;
    }
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) >> 3;
  }
  iVar12 = (*(int32_t*)rtguest::G((uint32_t)param_5));
  iVar1 = -iVar12;
  if (iVar3 < 0) {
    iVar3 = iVar3 + 3;
  }
  if (iVar12 < 0) {
    iVar12 = iVar12 + 7;
  }
  if (iVar1 < 0) {
    iVar1 = iVar1 + 7;
  }
  pfVar5 = (uint32_t)(param_2 + 0x114);
  (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(1) * 4)) = (iVar3 >> 2) + (iVar12 >> 3);
  (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(2) * 4)) = (iVar3 >> 2) + (iVar1 >> 3);
  uVar10 = 0;
  puVar6 = (uint32_t)(param_2 + 0x1b0);
  piVar7 = param_3;
  do {
    if ((*(int32_t*)rtguest::G((uint32_t)piVar7)) + iVar2 < (int)((*(float*)rtguest::G((uint32_t)pfVar5)) * 32768.0)) {
      uVar11 = uVar11 | 1 << (uVar10 & 0x1f);
      (*(float*)rtguest::G((uint32_t)pfVar5)) = (float)(*(int32_t*)rtguest::G((uint32_t)piVar7)) * 3.0517578e-05;
      if ((int)uVar10 < 5) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c0))) = 0;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0;
      }
      fVar13 = (*(float*)rtguest::G((uint32_t)param_4));
    }
    else {
      fVar13 = (*(float*)rtguest::G((uint32_t)param_4));
    }
    param_4 = param_4 + 1;
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    piVar7 = piVar7 + 0x4;  // T* element step
    uVar10 = uVar10 + 1;
    puVar6 = puVar6 + 0x4;  // T* element step
    pfVar5 = pfVar5 + 0x10;  // T* element step
    if ((int)(fVar13 * 32768.0) < iVar3 + iVar2 + 0x8000) {
      uVar11 = uVar11 | 0x100;
    }
  } while ((int)uVar10 < 7);
  uVar10 = 0;
  piVar8 = (uint32_t)(param_2 + 0x1c0);
  piVar7 = (uint32_t)(param_2 + 0x1e8);
  piVar9 = (uint32_t)(param_2 + 0x1dc);
  pfVar5 = (uint32_t)(param_2 + 0x114);
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_7;
  do {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_3));
    iVar12 = (int)((*(float*)rtguest::G((uint32_t)pfVar5)) * 32768.0);
    iVar2 = iVar12 - iVar3;
    if (iVar2 < 0) {
      (*(int32_t*)rtguest::G((uint32_t)param_3)) = iVar3 - (*(int32_t*)rtguest::G((uint32_t)piVar8));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
      if (((*(int32_t*)rtguest::G((uint32_t)piVar8)) == 0) && (-iVar2 <= iVar3)) {
        (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2;
      }
      else {
        iVar2 = iVar3 * 0x59;
        if (iVar2 < 0) {
          iVar2 = iVar2 + 0xfff;
        }
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar7)) - (iVar2 >> 0xc);
        iVar1 = iVar2 * 0xd;
        if (iVar1 < 0) {
          iVar1 = iVar1 + 0xf;
        }
        iVar4 = iVar2;
        if ((iVar1 >> 4 != 0) && (iVar4 = iVar1 >> 4, iVar2 < 1)) {
          iVar2 = iVar2 * 0xf;
          if (iVar2 < 0) {
            iVar2 = iVar2 + 0xf;
          }
          iVar4 = iVar2 >> 4;
        }
        iVar2 = 0x2000 - iVar3;
        if ((iVar3 + iVar4 < 0x2001) && (iVar2 = -iVar3, -1 < iVar3 + iVar4)) {
          iVar2 = iVar4;
        }
        (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)piVar9)) + (*(int32_t*)rtguest::G((uint32_t)piVar7));
      iVar3 = iVar12 - ((*(int32_t*)rtguest::G((uint32_t)param_3)) + (*(int32_t*)rtguest::G((uint32_t)piVar7)));
      (*(int32_t*)rtguest::G((uint32_t)param_3)) = (*(int32_t*)rtguest::G((uint32_t)param_3)) + (*(int32_t*)rtguest::G((uint32_t)piVar7));
      if (-1 < iVar3) {
        (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)piVar9)) + iVar3;
        if (((int)uVar11 >> (uVar10 & 0x1f) & 1U) == 0) {
          (*(int32_t*)rtguest::G((uint32_t)param_3)) = iVar12;
        }
        goto LAB_0021b310;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = (*(int32_t*)rtguest::G((uint32_t)piVar8)) + (*(int32_t*)rtguest::G((uint32_t)iStack_60));
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)param_3)) = iVar3 + iVar2;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
      iVar2 = iVar3 * 0x59;
      if (iVar2 < 0) {
        iVar2 = iVar2 + 0xfff;
      }
      iVar2 = ((*(int32_t*)rtguest::G((uint32_t)piVar7)) + (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(uVar10) * 4)) + 0x59) - (iVar2 >> 0xc);
      iVar12 = iVar2 * 0xd;
      if (iVar12 < 0) {
        iVar12 = iVar12 + 0xf;
      }
      iVar1 = iVar2;
      if ((iVar12 >> 4 != 0) && (iVar1 = iVar12 >> 4, iVar2 < 1)) {
        iVar2 = iVar2 * 0xf;
        if (iVar2 < 0) {
          iVar2 = iVar2 + 0xf;
        }
        iVar1 = iVar2 >> 4;
      }
      iVar2 = 0x2000 - iVar3;
      if ((iVar3 + iVar1 < 0x2001) && (iVar2 = -iVar3, -1 < iVar3 + iVar1)) {
        iVar2 = iVar1;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2;
      (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)piVar9)) + iVar2;
LAB_0021b310:
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
      if (iVar3 == 0) {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      }
      else if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x28))) & 0x40) == 0) {
        if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 0xfb0) == 0) {
          if (iVar3 < 0xb3) {
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
          }
          else {
            if (iVar3 < 0) {
              iVar3 = iVar3 + 0xf;
            }
            gs_controls_rumble_start(param_1,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 1,1,iVar3 >> 4);
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
          }
        }
        else {
          iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
        }
      }
      else {
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar3 + (*(int32_t*)rtguest::G((uint32_t)piVar8));
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
    }
    uVar10 = uVar10 + 1;
    piVar8 = piVar8 + 0x4;  // T* element step
    piVar7 = piVar7 + 0x4;  // T* element step
    piVar9 = piVar9 + 0x4;  // T* element step
    param_3 = param_3 + 1;
    pfVar5 = pfVar5 + 0x10;  // T* element step
    if (2 < (int)uVar10) {
      return uVar11;
    }
  } while( true );
}


RT_GUEST_FN(0x0021af28, guest::UpdateWheelSuspension)

}  // namespace guest
