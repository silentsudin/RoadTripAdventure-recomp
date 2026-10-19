// guest:0x00253e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyArenaWallCollisions(uint32_t param_1, ...) {
  bool bVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  fVar6 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  fVar2 = fVar6 - 82.0;
  if (fVar2 < 0.0) {
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    fVar3 = fVar5 - 182.0;
    if (fVar3 < 0.0) {
      if (fVar2 == 0.0) {
        fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
        goto LAB_002541ac;
      }
      if (fVar3 != 0.0) {
        fVar4 = fVar2;
        if (fVar2 < 0.0) {
          fVar4 = -fVar2;
        }
        if (fVar3 < 0.0) {
          if (-fVar3 < fVar4) {
            fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
            goto LAB_00253f4c;
          }
          fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          goto LAB_002541c8;
        }
        if (fVar4 <= fVar3) {
          fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          goto LAB_002541c8;
        }
        fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
LAB_00253f4c:
        fVar5 = fVar5 - fVar3;
        goto LAB_002541b0;
      }
      fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
LAB_002541c8:
      fVar3 = fVar3 * (*(float*)rtguest::G(0x333068));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = fVar6 - fVar2;
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30))) = fVar3;
    }
    else {
      fVar3 = fVar5 - 218.0;
      if (0.0 < fVar3) {
        if (fVar2 != 0.0) {
          if (fVar3 == 0.0) {
            fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          }
          else {
            fVar4 = fVar2;
            if (fVar2 < 0.0) {
              fVar4 = -fVar2;
            }
            if (fVar3 < 0.0) {
              if (-fVar3 < fVar4) {
                fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
                goto LAB_00253fec;
              }
              fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
            }
            else {
              if (fVar3 < fVar4) {
                fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
LAB_00253fec:
                fVar5 = fVar5 - fVar3;
                goto LAB_002541b0;
              }
              fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
            }
          }
          goto LAB_002541c8;
        }
        fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
      }
      else {
        fVar2 = fVar6 - 62.0;
        bVar1 = fVar2 < 0.0;
LAB_00254190:
        fVar3 = 0.0;
        if (!bVar1) goto LAB_002541e4;
        if (fVar2 != 0.0) {
          fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          goto LAB_002541c8;
        }
        fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
      }
LAB_002541ac:
      fVar5 = fVar5 - fVar3;
LAB_002541b0:
      fVar2 = fVar2 * (*(float*)rtguest::G(0x333064));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = fVar5;
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38))) = fVar2;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))) = 1;
  }
  else {
    fVar2 = fVar6 - 318.0;
    fVar5 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if (0.0 < fVar2) {
      fVar3 = fVar5 - 182.0;
      if (fVar3 < 0.0) {
        if (fVar2 == 0.0) {
          fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
          goto LAB_002541ac;
        }
        if (fVar3 == 0.0) {
          fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
        }
        else {
          fVar4 = fVar2;
          if (fVar2 < 0.0) {
            fVar4 = -fVar2;
          }
          if (fVar3 < 0.0) {
            if (-fVar3 < fVar4) {
              fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
              goto LAB_002540cc;
            }
            fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          }
          else {
            if (fVar3 < fVar4) {
              fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
LAB_002540cc:
              fVar5 = fVar5 - fVar3;
              goto LAB_002541b0;
            }
            fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          }
        }
      }
      else {
        fVar3 = fVar5 - 218.0;
        if (fVar3 <= 0.0) {
          fVar2 = fVar6 - 338.0;
          bVar1 = 0.0 < fVar2;
          goto LAB_00254190;
        }
        if (fVar2 == 0.0) {
          fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
          goto LAB_002541ac;
        }
        if (fVar3 == 0.0) {
          fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
        }
        else {
          fVar4 = fVar2;
          if (fVar2 < 0.0) {
            fVar4 = -fVar2;
          }
          if (fVar3 < 0.0) {
            if (-fVar3 < fVar4) {
              fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
              goto LAB_0025416c;
            }
            fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          }
          else {
            if (fVar3 < fVar4) {
              fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
LAB_0025416c:
              fVar5 = fVar5 - fVar3;
              goto LAB_002541b0;
            }
            fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
          }
        }
      }
      goto LAB_002541c8;
    }
  }
LAB_002541e4:
  fVar2 = (*(float*)rtguest::G(0x33306c));
  fVar6 = fVar5 - 122.0;
  if (0.0 <= fVar6) {
    fVar6 = fVar5 - 278.0;
    if (fVar6 <= 0.0) {
      fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24)));
      goto LAB_0025424c;
    }
    fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
  }
  else {
    fVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x4c))) = 1;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = fVar5 - fVar6;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38))) = fVar3 * fVar2;
  fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24)));
LAB_0025424c:
  fVar6 = (*(float*)rtguest::G(0x333074));
  if (2.0 < fVar2) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) - (*(float*)rtguest::G(0x333070));
    return 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24))) = 0x40000000;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34))) * fVar6;
  return 0;
}


RT_GUEST_FN(0x00253e90, guest::ApplyArenaWallCollisions)

}  // namespace guest
