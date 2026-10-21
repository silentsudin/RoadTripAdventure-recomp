// guest:0x00238b18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectAndBuildCamera(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_b0 = rtguest::allocFrame(4);
    uint32_t fStack_ac = rtguest::allocFrame(4);
    uint32_t fStack_a8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_90 = rtguest::allocFrame(4);
    uint32_t fStack_8c = rtguest::allocFrame(4);
    uint32_t fStack_88 = rtguest::allocFrame(4);
  byte bVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  bool bVar5;
  float fVar6;
  undefined8 uVar7;
  uint uVar8;
  int iVar9;
uint32_t pfVar10;
uint32_t pfVar11;
  uint uVar12;
uint32_t puVar13;
  float fVar14;
  float fVar15;
  
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  bVar5 = ((*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(iVar4 * 0x138) * 2)) & 0x40) != 0;
  uVar12 = (uint32_t)bVar5;
  iVar9 = uVar12 * 0x20;
  uVar8 = (uint32_t)bVar5;
  pfVar10 = (uint32_t)(0x0177e9f0 + iVar9);
  puVar13 = 0x177e6d0 + ((uVar12 * 2 + uVar8) * 8 + uVar8) * 0x10;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
  if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) == -1) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = 0x21ed90;
    RT_CALLRA(0x238bdc, guest::CameraWaitOrCutsceneState());
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xff;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
    }
    uVar12 = 0;
    pfVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a2a50u + ((int)((uint32_t)bVar1 << 0x18) >> 0x16))));
    while( true ) {
      (*(float*)rtguest::G((uint32_t)fStack_b0)) = (*(float*)rtguest::G((uint32_t)(uint32_t)0x177ace0 + (uint32_t)(iVar4 * 0x9c) * 4)) - (*(float*)rtguest::G((uint32_t)pfVar11));
      (*(float*)rtguest::G((uint32_t)fStack_ac)) = ((*(float*)rtguest::G((uint32_t)0x177ace4 + (uint32_t)(iVar4 * 0x9c) * 4)) + 1.0) - (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(1) * sizeof(float)));
      (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(iVar4 * 0x9c) * 4)) - (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(2) * sizeof(float)));
      fVar15 = (*(float*)rtguest::G((uint32_t)fStack_b0)) * (*(float*)rtguest::G((uint32_t)fStack_b0));
      fVar14 = (*(float*)rtguest::G((uint32_t)fStack_a8)) * (*(float*)rtguest::G((uint32_t)fStack_a8));
      (*(float*)rtguest::G((uint32_t)fStack_88)) = SQRT(fVar15 + (*(float*)rtguest::G((uint32_t)fStack_ac)) * (*(float*)rtguest::G((uint32_t)fStack_ac)) + fVar14);
      if (((uint32_t)(*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(7) * sizeof(float))) & 0x8000) != 0) break;
      if ((*(float*)rtguest::G((uint32_t)fStack_88)) < (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(3) * sizeof(float)))) {
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
        goto LAB_00238cc4;
      }
      pfVar11 = pfVar11 + 0x20;  // T* element step
      uVar12 = uVar12 + 1;
    }
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
LAB_00238cc4:
    if (bVar1 == uVar12) {
      uVar7 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar11));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (char)uVar12;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar10)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar11));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 2)));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 4)));
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea08 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 6)));
      uVar7 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar11));
    }
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar10)) = uVar7;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 2)));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 4)));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea08 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 6)));
    fVar6 = (*(float*)rtguest::G(0x332e38));
    uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar9)));
    if ((uVar12 & 2) != 0) {
      bVar5 = (*(float*)rtguest::G(0x332e38)) < (*(float*)rtguest::G((uint32_t)fStack_88)) * (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(4) * sizeof(float)));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = (*(float*)rtguest::G((uint32_t)fStack_88)) * (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(4) * sizeof(float)));
      if (bVar5) {
        (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = fVar6;
      }
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) < 500.0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = 0x43fa0000;
      }
    }
    if ((uVar12 & 0x1000) == 0) {
      if ((uVar12 & 0xc) == 0) {
        RT_CALLRA(0x238fd4, guest::buildCamera(puVar13,pfVar10,0,0));
      }
      else {
        RT_CALLRA(0x238ff0, guest::vu0UnitMatrix(uStack_c0));
        (*(float*)rtguest::G((uint32_t)pfVar10)) = 0.0;
        sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar11 + 0x1a)));
        uVar12 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar9)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f4 + iVar9))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar9))) = 0;
        (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (float)-(int)sVar3;
        if ((uVar12 & 0x10) != 0) {
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar9))) =
               (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar9))) + (*(int16_t*)rtguest::G((uint32_t)0x177ae24 + (uint32_t)(iVar4 * 0x138) * 2));  // 0x177ae24 halfword angle table (uint16 stores in fn/00218458.c:149, lh reads): element size 2, not 4
        }
        fVar15 = (*(float*)rtguest::G(0x332e40));
        if ((uVar12 & 2) != 0) {
          fVar14 = (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar11 + 0x1a))) * (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(4) * sizeof(float)));
          bVar5 = (*(float*)rtguest::G(0x332e40)) < fVar14;
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = fVar14;
          if (bVar5) {
            (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = fVar15;
          }
          if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) < 500.0) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = 0x43fa0000;
          }
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177e9fc + iVar9))) = 0x3f800000;
        if ((uVar12 & 4) == 0) {
          (*(float*)rtguest::G((uint32_t)fStack_90)) = (*(float*)rtguest::G((uint32_t)pfVar11));
        }
        else {
          (*(float*)rtguest::G((uint32_t)fStack_90)) = (*(float*)rtguest::G((uint32_t)(uint32_t)0x177ace0 + (uint32_t)(iVar4 * 0x9c) * 4));
        }
        (*(float*)rtguest::G((uint32_t)fStack_8c)) = (*(float*)rtguest::G((uint32_t)0x177ace4 + (uint32_t)(iVar4 * 0x9c) * 4));
        if ((uVar12 & 8) == 0) {
          (*(float*)rtguest::G((uint32_t)fStack_88)) = (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(2) * sizeof(float)));
        }
        else {
          (*(float*)rtguest::G((uint32_t)fStack_88)) = (*(float*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(iVar4 * 0x9c) * 4));
        }
        RT_CALLRA(0x2390e4, guest::buildCamera(puVar13,pfVar10,uStack_c0,uStack_c0));
      }
    }
    else {
      if (SQRT(fVar15 + fVar14) < (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar11 + 0x1a)))) {
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar10)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)pfVar11));
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 2)));
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 4)));
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea08 + iVar9))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 6)));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar9))) & 0x20) == 0) {
          sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar11 + 0x16)));
          sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 6)));
          (*(float*)rtguest::G((uint32_t)pfVar10)) = (*(float*)rtguest::G((uint32_t)pfVar10)) + (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 5)));
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f4 + iVar9))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f4 + iVar9))) + (float)(int)sVar3;
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) + (float)(int)sVar2;
        }
        else {
          sVar3 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar11 + 0x16)));
          sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 6)));
          (*(float*)rtguest::G((uint32_t)pfVar10)) = (float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar11 + 5)));
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f4 + iVar9))) = (float)(int)sVar3;
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9))) = (float)(int)sVar2;
        }
        (*(float*)rtguest::G((uint32_t)fStack_b0)) = (*(float*)rtguest::G((uint32_t)(uint32_t)0x177ace0 + (uint32_t)(iVar4 * 0x9c) * 4)) - (*(float*)rtguest::G((uint32_t)pfVar10));
        (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)0x177ace8 + (uint32_t)(iVar4 * 0x9c) * 4));
        (*(float*)rtguest::G((uint32_t)fStack_ac)) = ((*(float*)rtguest::G((uint32_t)0x177ace4 + (uint32_t)(iVar4 * 0x9c) * 4)) + 1.0) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f4 + iVar9)))
        ;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea04 + iVar9))) = 0;
        (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)fStack_a8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177e9f8 + iVar9)));
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar9))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea08 + iVar9))) = 0;
        (*(float*)rtguest::G((uint32_t)fStack_88)) = SQRT((*(float*)rtguest::G((uint32_t)fStack_b0)) * (*(float*)rtguest::G((uint32_t)fStack_b0)) + (*(float*)rtguest::G((uint32_t)fStack_ac)) * (*(float*)rtguest::G((uint32_t)fStack_ac)) + (*(float*)rtguest::G((uint32_t)fStack_a8)) * (*(float*)rtguest::G((uint32_t)fStack_a8)));
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar9))) = 0;
        fVar15 = (*(float*)rtguest::G(0x332e3c));
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar9))) & 2) != 0) {
          bVar5 = (*(float*)rtguest::G(0x332e3c)) < (*(float*)rtguest::G((uint32_t)fStack_88)) * (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(4) * sizeof(float)));
          (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = (*(float*)rtguest::G((uint32_t)fStack_88)) * (*(float*)rtguest::G((uint32_t)pfVar11 + (uint32_t)(4) * sizeof(float)));
          if (bVar5) {
            (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = fVar15;
          }
          if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) < 500.0) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea00 + iVar9))) = 0x43fa0000;
          }
        }
        fVar15 = (*(float*)rtguest::G((uint32_t)fStack_b0)) * (*(float*)rtguest::G((uint32_t)fStack_b0)) + (*(float*)rtguest::G((uint32_t)fStack_a8)) * (*(float*)rtguest::G((uint32_t)fStack_a8));
      }
      else {
        fVar15 = fVar15 + fVar14;
      }
      (*(float*)rtguest::G((uint32_t)fStack_ac)) = rtguest::fdiv((*(float*)rtguest::G((uint32_t)fStack_ac)), (*(float*)rtguest::G((uint32_t)fStack_88)));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x3f800000;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
      (*(float*)rtguest::G((uint32_t)fStack_90)) = 0.0;
      (*(float*)rtguest::G((uint32_t)fStack_b0)) = rtguest::fdiv((*(float*)rtguest::G((uint32_t)fStack_b0)), (*(float*)rtguest::G((uint32_t)fStack_88)));
      (*(float*)rtguest::G((uint32_t)fStack_a8)) = rtguest::fdiv((*(float*)rtguest::G((uint32_t)fStack_a8)), (*(float*)rtguest::G((uint32_t)fStack_88)));
      (*(float*)rtguest::G((uint32_t)fStack_88)) = rtguest::fdiv(SQRT(fVar15), (*(float*)rtguest::G((uint32_t)fStack_88)));
      (*(float*)rtguest::G((uint32_t)fStack_8c)) = (*(float*)rtguest::G((uint32_t)fStack_ac));
      RT_CALLRA(0x239100, guest::buildCamera(puVar13,pfVar10,uStack_c0,0));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00238b18, guest::SelectAndBuildCamera)

}  // namespace guest
