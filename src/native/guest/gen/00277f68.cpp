// guest:0x00277f68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCd_cd_read_intr(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
uint32_t piVar1;
  int iVar2;
uint32_t puVar3;
  int iVar4;
  int iVar5;
uint32_t piVar6;
  
  piVar6 = (uint32_t)(param_1 | 0x20000000);
  if (0 < (*(int32_t*)rtguest::G((uint32_t)piVar6))) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(2) * sizeof(int32_t)));
    iVar4 = 0;
    if (0 < (*(int32_t*)rtguest::G((uint32_t)piVar6))) {
      piVar1 = piVar6 + 4;
      do {
        puVar3 = (uint32_t)(iVar2 + iVar4);
        iVar4 = iVar4 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = (char)(*(int32_t*)rtguest::G((uint32_t)piVar1));
        piVar1 = (uint32_t)((int)(piVar6 + 4) + iVar4);
      } while (iVar4 < (*(int32_t*)rtguest::G((uint32_t)piVar6)));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(1) * sizeof(int32_t)));
      goto LAB_00277fc4;
    }
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(1) * sizeof(int32_t)));
LAB_00277fc4:
  if (0 < iVar2) {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(3) * sizeof(int32_t)));
    iVar5 = 0;
    if (0 < iVar2) {
      piVar1 = piVar6 + 0x14 * sizeof(int32_t);  // fn:40 int* elem step
      do {
        puVar3 = (uint32_t)(iVar4 + iVar5);
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar3)) = (char)(*(int32_t*)rtguest::G((uint32_t)piVar1));
        piVar1 = (uint32_t)((int)(piVar6 + 0x14) + iVar5);
      } while (iVar5 < (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(1) * sizeof(int32_t))));
    }
  }
  _sceCd_cd_callback(0x2e6f54);
  return 0;
}


RT_GUEST_FN(0x00277f68, guest::_sceCd_cd_read_intr)

}  // namespace guest
