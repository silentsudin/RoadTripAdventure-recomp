// guest:0x00284008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __mdiff(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  uint uVar2;
uint32_t piVar3;
uint32_t piVar4;
  long lVar5;
  undefined8 uVar6;
  int iVar7;
uint32_t puVar8;
uint32_t piVar9;
uint32_t puVar10;
uint32_t puVar11;
  int iVar12;
  int iVar13;
uint32_t puVar14;
  
  lVar5 = __mcmp(param_2,param_3);
  if (lVar5 == 0) {
    uVar6 = _Balloc(param_1,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar6 + 0x10))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar6 + 0x14))) = 0;
  }
  else {
    uVar6 = param_3;
    if (lVar5 >= 0) {
      uVar6 = param_2;
      param_2 = param_3;
    }
    iVar13 = (int)uVar6;
    uVar6 = _Balloc(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4))));
    puVar10 = (uint32_t)(iVar13 + 0x14);
    puVar11 = (uint32_t)((int)param_2 + 0x14);
    iVar12 = (int)uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0xc))) = (uint32_t)(lVar5 < 0);
    iVar7 = 0;
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10)));
    puVar14 = puVar10 + iVar13;
    puVar8 = puVar11 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10)));
    piVar4 = (uint32_t)(iVar12 + 0x14);
    do {
      piVar9 = piVar4;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
      puVar10 = puVar10 + 1;
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar11));
      puVar11 = puVar11 + 1;
      iVar7 = ((uVar1 & 0xffff) - (uVar2 & 0xffff)) + iVar7;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)piVar9)) = (short)iVar7;
      iVar7 = ((uVar1 >> 0x10) - (uVar2 >> 0x10)) + (iVar7 >> 0x10);
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar9 + 2))) = (short)iVar7;
      iVar7 = iVar7 >> 0x10;
      piVar4 = piVar9 + 1;
    } while (puVar11 < puVar8);
    for (; piVar3 = piVar4, puVar10 < puVar14; puVar10 = puVar10 + 1) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
      iVar7 = (uVar1 & 0xffff) + iVar7;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)piVar3)) = (short)iVar7;
      iVar7 = (uVar1 >> 0x10) + (iVar7 >> 0x10);
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar3 + 2))) = (short)iVar7;
      iVar7 = iVar7 >> 0x10;
      piVar4 = piVar3 + 1;
      piVar9 = piVar3;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)piVar9)) == 0) {
      do {
        piVar9 = piVar9 + -1;
        iVar13 = iVar13 + -1;
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar9)) == 0);
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x10))) = iVar13;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x10))) = iVar13;
    }
  }
  return uVar6;
}


RT_GUEST_FN(0x00284008, guest::__mdiff)

}  // namespace guest
