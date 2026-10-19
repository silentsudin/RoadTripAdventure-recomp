// guest:0x00241f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateNearbyCarEffect(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  char cVar2;
  int iVar3;
uint32_t puVar4;
  int iVar5;
uint32_t pfVar6;
  int iVar7;
  float fVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  float fVar11;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 1) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0;
  }
  fVar8 = 1600.0;
  puVar4 = 0x177ac50;
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc;
  iVar7 = 0x17;
  do {
    if (((((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x33))) != 0) &&
         (ABS((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar5))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x12)))) < fVar8)) &&
        ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb8 + iVar5))) - 20.0 < (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x13))))) &&
       ((*(float*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x13))) < (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb8 + iVar5))) + 20.0)) {
      fVar8 = ABS((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar5))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x12))));
    }
    iVar7 = iVar7 + -1;
    puVar4 = puVar4 + 0x4e;
  } while (-1 < iVar7);
  fVar11 = (fVar8 - 5.0) / 15.0;
  if (fVar8 < 5.0) {
    fVar11 = 0.0;
  }
  if (20.0 < fVar8) {
    fVar11 = 1.0;
  }
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  iVar7 = 0;
  pfVar6 = (uint32_t)0x177ace0;
  iVar5 = 0;
  do {
    fVar8 = (float)GetSpeedFromF15Vector((int)0x0177ad40 + iVar5);
    if (0x3c < (int)fVar8) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
      if (((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb8 + iVar3 * 0xc))) - 20.0 < (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ace8 + iVar5))))
         && ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ace8 + iVar5))) < (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb8 + iVar3 * 0xc))) + 20.0
            )) {
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) != '\x02') {
          if (((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) - 20.0 <=
               (*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x177ace0 + iVar5)))) &&
             ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x177ace0 + iVar5))) <
              (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) + 20.0)) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 1;
          }
        }
        if ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) - 5.0 <= (*(float*)rtguest::G((uint32_t)pfVar6))) {
          if ((*(float*)rtguest::G((uint32_t)pfVar6)) < (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) + 5.0) {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 2;
          }
          cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
        }
        else {
          cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
        }
        if (cVar2 == '\x02') {
          if ((*(float*)rtguest::G((uint32_t)pfVar6)) < (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) + 20.0) {
            if ((*(float*)rtguest::G((uint32_t)pfVar6)) < (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar3 * 0xc))) - 20.0) {
              (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0;
            }
          }
          else {
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0;
          }
        }
      }
    }
    iVar7 = iVar7 + 1;
    pfVar6 = pfVar6 + 0x9c;
    iVar5 = iVar5 + 0x270;
  } while ((iVar7 < 2) && ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) != 0 || (iVar7 != 1))));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x48) != 0) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    goto LAB_002422bc;
  }
  cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  if (cVar1 != cVar2) {
    if (cVar2 == '\x01') {
      PlayUiSound(0x307);
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (cVar2 != '\x02') {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    PlayUiSound(0x308);
  }
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
LAB_002422bc:
  iVar7 = iVar5 << 2;
  if (4 < iVar5) {
    if (iVar5 < 6) {
      iVar7 = iVar5 * 4 + 2;
    }
    else {
      iVar7 = iVar5 * 4 + -0x18;
    }
  }
  vu0UnitMatrix(0x0177eb10 + iVar7 * 0x40);
  vu0UnitMatrix(0x0177eb50 + iVar7 * 0x40);
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc;
  fVar11 = (1.0 - fVar11) * 5.0;
  fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb8 + iVar5)));
  uVar10 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a3fb0 + iVar5)));
  uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2a3fb4)));
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb40 + (uint32_t)(iVar7 * 0x10) * 4)) = uVar10;
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb44 + (uint32_t)(iVar7 * 0x10) * 4)) = uVar9;
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb48 + (uint32_t)(iVar7 * 0x10) * 4)) = (fVar8 - fVar11) - 5.0;
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb88 + (uint32_t)(iVar7 * 0x10) * 4)) = fVar8 + fVar11 + 5.0;
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb80 + (uint32_t)(iVar7 * 0x10) * 4)) = uVar10;
  (*(uint32_t*)rtguest::G((uint32_t)0x177eb84 + (uint32_t)(iVar7 * 0x10) * 4)) = uVar9;
  return 0;
}


RT_GUEST_FN(0x00241f68, guest::UpdateNearbyCarEffect)

}  // namespace guest
