// guest:0x00252608 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitGolfCourseTable(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  int iVar2;
uint32_t puVar3;
uint32_t pcVar4;
  int iVar5;
uint32_t piVar6;
  
  iVar5 = 0;
  piVar6 = (uint32_t)(param_1 + 0x800c);
  do {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2));
    param_2 = param_2 + (uint32_t)(1) * sizeof(int32_t);
    iVar5 = iVar5 + 1;
    (*(int32_t*)rtguest::G((uint32_t)piVar6)) = iVar2;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) + iVar2;
    piVar6 = piVar6 + (uint32_t)(1) * sizeof(int32_t);
  } while (iVar5 < 9);
  iVar5 = 0;
  piVar6 = (uint32_t)(param_1 + 0x8034);
  do {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_3));
    param_3 = param_3 + (uint32_t)(1) * sizeof(int32_t);
    iVar5 = iVar5 + 1;
    (*(int32_t*)rtguest::G((uint32_t)piVar6)) = iVar2;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8058))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8058))) + iVar2;
    piVar6 = piVar6 + (uint32_t)(1) * sizeof(int32_t);
  } while (iVar5 < 9);
  puVar3 = (uint32_t)(param_1 + 0x80a4);
  iVar5 = 8;
  do {
    iVar5 = iVar5 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0x7f;
    puVar3 = puVar3 + (uint32_t)(-1) * sizeof(uint32_t);
  } while (-1 < iVar5);
  piVar6 = (uint32_t)(param_1 + 0x805c);
  iVar5 = 9;
  pcVar4 = 0x01780a30;
  do {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar4));
    pcVar4 = pcVar4 + 1;
    iVar5 = iVar5 + -1;
    (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (int)cVar1;
    piVar6 = piVar6 + (uint32_t)(1) * sizeof(int32_t);
  } while (-1 < iVar5);
  puVar3 = (uint32_t)(param_1 + 0x80d0);
  iVar5 = 9;
  do {
    iVar5 = iVar5 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0;
    puVar3 = puVar3 + (uint32_t)(-1) * sizeof(uint32_t);
  } while (-1 < iVar5);
  return 0;
}


RT_GUEST_FN(0x00252608, guest::InitGolfCourseTable)

}  // namespace guest
