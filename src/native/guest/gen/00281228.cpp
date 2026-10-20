// guest:0x00281228 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t quorem(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  uint uVar1;
  uint uVar2;
  uint uVar3;
  long lVar4;
  int iVar5;
  uint uVar6;
uint32_t puVar7;
uint32_t puVar8;
uint32_t puVar9;
uint32_t puVar10;
  int iVar11;
uint32_t puVar12;
  int iVar13;
uint32_t puVar14;
  int iVar15;
  
  iVar11 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10)));
  iVar15 = (int)param_1;
  iVar13 = 0;
  if (iVar11 <= (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10)))) {
    iVar11 = iVar11 + -1;
    puVar9 = (uint32_t)((int)param_2 + 0x14);
    puVar12 = (uint32_t)(iVar15 + 0x14);
    puVar14 = puVar9 + iVar11;
    puVar7 = puVar12 + iVar11;
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
    iVar13 = (int)uVar1 / (int)((*(uint32_t*)rtguest::G((uint32_t)puVar14)) + 1);
    if ((*(uint32_t*)rtguest::G((uint32_t)puVar14)) + 1 == 0) {
      trap(7);
    }
    iVar5 = 0;
    if (iVar13 != 0) {
      uVar3 = 0;
      puVar10 = puVar9;
      puVar8 = puVar12;
      do {
        uVar6 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
        puVar10 = puVar10 + 0x4;  // T* element step
        uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar8));
        uVar3 = (uVar6 & 0xffff) * iVar13 + uVar3;
        uVar6 = (uVar6 >> 0x10) * iVar13 + (uVar3 >> 0x10);
        iVar5 = ((uVar2 & 0xffff) - (uVar3 & 0xffff)) + iVar5;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)puVar8)) = (short)iVar5;
        iVar5 = ((uVar2 >> 0x10) - (uVar6 & 0xffff)) + (iVar5 >> 0x10);
        uVar3 = uVar6 >> 0x10;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 2))) = (short)iVar5;
        puVar8 = puVar8 + 0x4;  // T* element step
        iVar5 = iVar5 >> 0x10;
      } while (puVar10 <= puVar14);
      if (uVar1 == 0) {
        while (puVar7 = puVar7 + -1, puVar12 < puVar7) {
          if ((*(uint32_t*)rtguest::G((uint32_t)puVar7)) != 0) {
            (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10))) = iVar11;
            goto LAB_0028135c;
          }
          iVar11 = iVar11 + -1;
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10))) = iVar11;
      }
    }
LAB_0028135c:
    lVar4 = RT_CALLRA(0x28135c, guest::__mcmp(param_1,param_2));
    if (-1 < lVar4) {
      iVar13 = iVar13 + 1;
      iVar5 = 0;
      puVar7 = puVar12;
      do {
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        puVar9 = puVar9 + 0x4;  // T* element step
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
        iVar5 = ((uVar3 & 0xffff) - (uVar1 & 0xffff)) + iVar5;
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)puVar7)) = (short)iVar5;
        iVar5 = ((uVar3 >> 0x10) - (uVar1 >> 0x10)) + (iVar5 >> 0x10);
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar7 + 2))) = (short)iVar5;
        puVar7 = puVar7 + 0x4;  // T* element step
        iVar5 = iVar5 >> 0x10;
      } while (puVar9 <= puVar14);
      puVar7 = puVar12 + iVar11;
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar7)) == 0) {
        while (puVar7 = puVar7 + -1, puVar12 < puVar7) {
          if ((*(uint32_t*)rtguest::G((uint32_t)puVar7)) != 0) {
            (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10))) = iVar11;
            return iVar13;
          }
          iVar11 = iVar11 + -1;
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x10))) = iVar11;
      }
    }
  }
  return iVar13;
}


RT_GUEST_FN(0x00281228, guest::quorem)

}  // namespace guest
