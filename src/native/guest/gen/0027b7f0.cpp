// guest:0x0027b7f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t _fpadd_parts(uint64_t param_1, uint64_t param_2, uint64_t param_3, ...) {
  bool bVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t puVar5;
uint32_t puVar6;
  uint uVar7;
  uint uVar8;
  ulong uVar9;
  ulong uVar10;
uint32_t puVar11;
  
  puVar6 = (uint32_t)param_3;
  puVar11 = (uint32_t)param_1;
  uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar11));
  uVar3 = param_1;
  if (1 < uVar8) {
    puVar5 = (uint32_t)param_2;
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar5));
    uVar3 = param_2;
    if (1 < uVar7) {
      if (uVar8 == 4) {
        if ((uVar7 == 4) && ((*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint32_t))) != (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t))))) {
          return 0x331f70;
        }
        return param_1;
      }
      if (uVar7 != 4) {
        if (uVar7 == 2) {
          if (uVar8 != 2) {
            return param_1;
          }
          (*(uint64_t*)rtguest::G((uint32_t)puVar6)) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)puVar11));
          (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 2)));
          (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 4)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint32_t))) & (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t)));
          return param_3;
        }
        if (uVar8 != 2) {
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(2) * sizeof(uint32_t)));
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint32_t)));
          uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar11 + 4)));
          iVar2 = uVar8 - uVar7;
          if (iVar2 < 0) {
            iVar2 = -iVar2;
          }
          uVar9 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 4)));
          if (iVar2 < 0x40) {
            bVar1 = (int)uVar8 < (int)uVar7;
            if ((int)uVar7 < (int)uVar8) {
              iVar2 = uVar8 - uVar7;
              do {
                iVar2 = iVar2 + -1;
                uVar9 = uVar9 & 1 | uVar9 >> 1;
              } while (iVar2 != 0);
              bVar1 = false;
              uVar7 = uVar8;
            }
            if (bVar1) {
              do {
                uVar8 = uVar8 + 1;
                uVar10 = uVar10 & 1 | uVar10 >> 1;
              } while ((int)uVar8 < (int)uVar7);
              uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint32_t)));
            }
            else {
              uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint32_t)));
            }
          }
          else {
            if ((int)uVar7 < (int)uVar8) {
              uVar9 = 0;
            }
            else {
              uVar10 = 0;
              uVar8 = uVar7;
            }
            uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar11 + (uint32_t)(1) * sizeof(uint32_t)));
          }
          if (uVar7 == (*(uint32_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint32_t)))) {
            uVar10 = uVar10 + uVar9;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 4))) = uVar7;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) = uVar8;
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10;
          }
          else {
            lVar4 = uVar9 - uVar10;
            if (uVar7 == 0) {
              lVar4 = uVar10 - uVar9;
            }
            if (lVar4 < 0) {
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 4))) = 1;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) = uVar8;
              (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = -lVar4;
            }
            else {
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) = uVar8;
              (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = lVar4;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 4))) = 0;
            }
            uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t)));
            while (uVar10 - 1 < 0xfffffffffffffff) {
              uVar10 = uVar10 * 2;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) + -1;
              (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10;
            }
          }
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar6)) = 3;
          uVar3 = param_3;
          if (0x1fffffffffffffff < uVar10) {
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10 & 1 | uVar10 >> 1;
            (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 1))) + 1;
          }
        }
      }
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x0027b7f0, guest::_fpadd_parts)

}  // namespace guest
