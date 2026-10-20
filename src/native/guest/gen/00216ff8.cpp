// guest:0x00216ff8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitPauseMenuOptions(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  ushort uVar2;
  int iVar3;
uint32_t piVar4;
uint32_t piVar5;
uint32_t puVar6;
  int iVar7;
  uint uVar8;
uint32_t puVar9;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_3)), 0, 0x83a0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8084))) = 1;
  piVar4 = 0x0029ca30;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8080))) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8020))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8024))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8028))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x802c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8030))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8034))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8038))) = 0;
  iVar7 = 0;
  if ((*(uint32_t*)rtguest::G(0x29ca30)) != -1) {
    piVar5 = (uint32_t)((int)param_3 + 0x803c);
    iVar3 = (*(uint32_t*)rtguest::G(0x29ca30));
    do {
      piVar4 = piVar4 + (uint32_t)(1) * sizeof(int32_t);
      iVar7 = iVar7 + 1;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = iVar3;
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar4));
      piVar5 = piVar5 + (uint32_t)(1) * sizeof(int32_t);
    } while (iVar3 != -1);
  }
  uVar8 = 0;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x807c))) = iVar7 + -1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 33000))) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8088))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x808c))) = 0;
  puVar9 = 0x0029ca50;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8090))) = 0;
  puVar6 = (uint32_t)((int)param_3 + 0x80a8);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8094))) = 0;
  iVar7 = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x8098))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x809c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x80a0))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x80ec))) = 0;
  uVar2 = (*(uint16_t*)rtguest::G(0x1780a3a));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x80a4))) = 1;
  do {
    uVar1 = uVar8 & 0x1f;
    uVar8 = uVar8 + 1;
    if (((int)(uint32_t)uVar2 >> uVar1 & 1U) == 0) {
      iVar7 = iVar7 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
      puVar6 = puVar6 + (uint32_t)(1) * sizeof(uint32_t);
    }
    puVar9 = puVar9 + (uint32_t)(1) * sizeof(uint32_t);
  } while ((int)uVar8 < 10);
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x80e4))) = iVar7 + -1;
  return 0;
}


RT_GUEST_FN(0x00216ff8, guest::InitPauseMenuOptions)

}  // namespace guest
