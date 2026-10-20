// guest:0x00284008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __mdiff(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
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
  
  lVar5 = RT_CALLRA(0x284030, guest::__mcmp(param_2,param_3));
  if (lVar5 == 0) {
    uVar6 = RT_CALLRA(0x284044, guest::_Balloc(param_1,0));
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
    uVar6 = RT_CALLRA(0x28407c, guest::_Balloc(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 4)))));
    puVar10 = (uint32_t)(iVar13 + 0x14);
    puVar11 = (uint32_t)((int)param_2 + 0x14);
    iVar12 = (int)uVar6;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0xc))) = (uint32_t)(lVar5 < 0);
    iVar7 = 0;
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar13 + 0x10)));
    puVar14 = puVar10 + (uint32_t)(iVar13) * sizeof(uint32_t);
    puVar8 = puVar11 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10)));
    piVar4 = (uint32_t)(iVar12 + 0x14);
    do {
      piVar9 = piVar4;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
      puVar10 = puVar10 + (uint32_t)(1) * sizeof(uint32_t);
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar11));
      puVar11 = puVar11 + (uint32_t)(1) * sizeof(uint32_t);
      iVar7 = ((uVar1 & 0xffff) - (uVar2 & 0xffff)) + iVar7;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)piVar9)) = (short)iVar7;
      iVar7 = ((uVar1 >> 0x10) - (uVar2 >> 0x10)) + (iVar7 >> 0x10);
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar9 + 2))) = (short)iVar7;
      iVar7 = iVar7 >> 0x10;
      piVar4 = piVar9 + (uint32_t)(1) * sizeof(uint32_t);
    } while (puVar11 < puVar8);
    for (; piVar3 = piVar4, puVar10 < puVar14; puVar10 = puVar10 + (uint32_t)(1) * sizeof(uint32_t)) {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
      iVar7 = (uVar1 & 0xffff) + iVar7;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)piVar3)) = (short)iVar7;
      iVar7 = (uVar1 >> 0x10) + (iVar7 >> 0x10);
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar3 + 2))) = (short)iVar7;
      iVar7 = iVar7 >> 0x10;
      piVar4 = piVar3 + (uint32_t)(1) * sizeof(uint32_t);
      piVar9 = piVar3;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)piVar9)) == 0) {
      do {
        piVar9 = piVar9 - (uint32_t)(1) * sizeof(uint32_t);
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
