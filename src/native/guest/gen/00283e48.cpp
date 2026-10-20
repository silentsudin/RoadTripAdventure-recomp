// guest:0x00283e48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _lshift(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  undefined8 uVar1;
  int iVar2;
  uint uVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
uint32_t puVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  iVar9 = (int)param_3 >> 5;
  iVar11 = (int)param_2;
  iVar2 = iVar9 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x10)));
  iVar10 = iVar2 + 1;
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 4)));
  for (iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 8))); iVar6 < iVar10; iVar6 = iVar6 << 1) {
    iVar5 = iVar5 + 1;
  }
  uVar1 = RT_CALLRA(0x283eb4, guest::_Balloc(param_1,iVar5));
  puVar8 = (uint32_t)((int)uVar1 + 0x14);
  if (0 < iVar9) {
    do {
      iVar9 = iVar9 + -1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 0;
      puVar8 = puVar8 + (uint32_t)(1) * sizeof(uint32_t);
    } while (iVar9 != 0);
  }
  puVar4 = (uint32_t)(iVar11 + 0x14);
  param_3 = param_3 & 0x1f;
  puVar7 = puVar4 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x10)));
  if (param_3 == 0) {
    do {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
      puVar4 = puVar4 + (uint32_t)(1) * sizeof(uint32_t);
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = uVar3;
      puVar8 = puVar8 + (uint32_t)(1) * sizeof(uint32_t);
    } while (puVar4 < puVar7);
  }
  else {
    uVar3 = 0;
    do {
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4)) << param_3 | uVar3;
      puVar8 = puVar8 + (uint32_t)(1) * sizeof(uint32_t);
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
      puVar4 = puVar4 + (uint32_t)(1) * sizeof(uint32_t);
      uVar3 = uVar3 >> (0x20 - param_3 & 0x1f);
    } while (puVar4 < puVar7);
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = uVar3;
    if (uVar3 != 0) {
      iVar10 = iVar2 + 2;
    }
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)uVar1 + 0x10))) = iVar10 + -1;
  RT_CALLRA(0x283f70, guest::_Bfree(param_1,param_2));
  return uVar1;
}


RT_GUEST_FN(0x00283e48, guest::_lshift)

}  // namespace guest
