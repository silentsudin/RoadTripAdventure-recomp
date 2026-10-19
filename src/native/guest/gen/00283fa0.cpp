// guest:0x00283fa0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __mcmp(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
uint32_t puVar3;
uint32_t puVar4;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) - iVar2;
  if (iVar1 == 0) {
    puVar3 = (uint32_t)(param_1 + 0x14) + iVar2;
    puVar4 = (uint32_t)(param_2 + 0x14 + iVar2 * 4);
    do {
      puVar3 = puVar3 + -1;
      puVar4 = puVar4 + -1;
      iVar2 = -1;
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar4)) <= (*(uint32_t*)rtguest::G((uint32_t)puVar3))) {
        iVar2 = 1;
      }
      if ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) != (*(uint32_t*)rtguest::G((uint32_t)puVar4))) {
      }
    } while ((uint32_t)(param_1 + 0x14) < puVar3);
    iVar1 = 0;
  }
}


RT_GUEST_FN(0x00283fa0, guest::__mcmp)

}  // namespace guest
