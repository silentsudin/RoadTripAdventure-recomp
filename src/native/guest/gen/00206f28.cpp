// guest:0x00206f28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t IntersectPointWithMesh(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, uint32_t param_8, ...) {
  uint uVar1;
uint32_t pfVar2;
uint32_t pfVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  
  iVar6 = (int)param_4 + -1;
  if (param_4 == 0) {
    return param_8;
  }
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  do {
    pfVar3 = (uint32_t)(param_1 + 4);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
    uVar4 = uVar4 & 0x7fff;
    iVar5 = uVar4 - 2;
    param_1 = param_1 + uVar4 * 8 + -4;
    pfVar2 = pfVar3 + uVar4 * 4;
    if (iVar5 != 0) {
      do {
        fVar13 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(2) * sizeof(float)));
        fVar9 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(6) * sizeof(float)));
        fVar10 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(4) * sizeof(float)));
        fVar14 = (*(float*)rtguest::G((uint32_t)param_2));
        fVar11 = (*(float*)rtguest::G((uint32_t)pfVar3));
        fVar12 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
        if (0.0 <= (fVar9 - fVar13) * (fVar14 - fVar10) - (fVar10 - fVar11) * (fVar12 - fVar9)) {
          fVar8 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(8) * sizeof(float)));
          fVar7 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(10) * sizeof(float)));
          if (0.0 <= (fVar7 - fVar9) * (fVar14 - fVar8) - (fVar8 - fVar10) * (fVar12 - fVar7)) {
            if (0.0 <= (fVar13 - fVar7) * (fVar14 - fVar11) - (fVar11 - fVar8) * (fVar12 - fVar13))
            {
              fVar9 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(1) * sizeof(float))) - ((fVar14 - fVar11) * (*(float*)rtguest::G((uint32_t)pfVar2)) + (fVar12 - fVar13) * (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float))));
              if ((int)uVar1 < 0) {
                fVar10 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float)));
                if ((fVar10 < fVar9) && (fVar9 < (*(float*)rtguest::G((uint32_t)param_6)))) {
                  (*(float*)rtguest::G((uint32_t)param_6)) = fVar9;
                  fVar10 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float)));
                }
                if ((fVar9 < fVar10) && ((*(float*)rtguest::G((uint32_t)param_5)) < fVar9)) {
                  (*(float*)rtguest::G((uint32_t)param_5)) = fVar9;
                }
              }
              else if ((uVar1 & 0x10000000) == 0) {
                if ((fVar9 < (*(float*)rtguest::G((uint32_t)param_6))) && ((*(float*)rtguest::G((uint32_t)param_5)) < fVar9)) {
                  (*(float*)rtguest::G((uint32_t)param_5)) = fVar9;
                  (*(uint32_t*)rtguest::G((uint32_t)param_3)) = pfVar2;
                  param_8 = uVar1;
                }
              }
              else {
                (*(float*)rtguest::G((uint32_t)param_7)) = fVar9;
              }
            }
          }
        }
        if (iVar5 == 1) break;
        fVar13 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(6) * sizeof(float)));
        fVar9 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(10) * sizeof(float)));
        fVar10 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(8) * sizeof(float)));
        fVar14 = (*(float*)rtguest::G((uint32_t)param_2));
        fVar11 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(4) * sizeof(float)));
        fVar12 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float)));
        if ((fVar9 - fVar13) * (fVar14 - fVar10) - (fVar10 - fVar11) * (fVar12 - fVar9) <= 0.0) {
          fVar8 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(0xc) * sizeof(float)));
          fVar7 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(0xe) * sizeof(float)));
          if ((fVar7 - fVar9) * (fVar14 - fVar8) - (fVar8 - fVar10) * (fVar12 - fVar7) <= 0.0) {
            if ((fVar13 - fVar7) * (fVar14 - fVar11) - (fVar11 - fVar8) * (fVar12 - fVar13) <= 0.0)
            {
              fVar9 = (*(float*)rtguest::G((uint32_t)pfVar3 + (uint32_t)(5) * sizeof(float))) - ((fVar14 - fVar11) * (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(4) * sizeof(float))) + (fVar12 - fVar13) * (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(6) * sizeof(float))));
              if ((int)uVar1 < 0) {
                fVar10 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float)));
                if ((fVar10 < fVar9) && (fVar9 < (*(float*)rtguest::G((uint32_t)param_6)))) {
                  (*(float*)rtguest::G((uint32_t)param_6)) = fVar9;
                  fVar10 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float)));
                }
                if ((fVar9 < fVar10) && ((*(float*)rtguest::G((uint32_t)param_5)) < fVar9)) {
                  (*(float*)rtguest::G((uint32_t)param_5)) = fVar9;
                }
              }
              else if ((uVar1 & 0x10000000) == 0) {
                if ((fVar9 < (*(float*)rtguest::G((uint32_t)param_6))) && ((*(float*)rtguest::G((uint32_t)param_5)) < fVar9)) {
                  (*(float*)rtguest::G((uint32_t)param_5)) = fVar9;
                  (*(uint32_t*)rtguest::G((uint32_t)param_3)) = pfVar2 + 4;
                  param_8 = uVar1;
                }
              }
              else {
                (*(float*)rtguest::G((uint32_t)param_7)) = fVar9;
              }
            }
          }
        }
        iVar5 = iVar5 + -2;
        pfVar2 = pfVar2 + 8;
        pfVar3 = pfVar3 + 8;
      } while (iVar5 != 0);
    }
    iVar6 = iVar6 + -1;
    if (iVar6 == -1) {
      return param_8;
    }
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  } while( true );
}


RT_GUEST_FN(0x00206f28, guest::IntersectPointWithMesh)

}  // namespace guest
