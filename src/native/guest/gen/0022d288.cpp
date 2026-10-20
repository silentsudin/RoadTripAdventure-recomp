// guest:0x0022d288 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetRecordTables(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  byte bVar1;
uint32_t puVar2;
  int iVar3;
  
  puVar2 = 0x01787068;
  iVar3 = 0x17;
  do {
    iVar3 = iVar3 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0x7fffffff;
    puVar2 = puVar2 + 0x4;  // T* element step
  } while (-1 < iVar3);
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 10)));
  if ((bVar1 == 0) && ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) < 0x20)) {
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) < 0x1a) {
      iVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) * 4 + 0x1040;
    }
    else {
      iVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) * 4 + 0x1038;
    }
    (*(uint32_t*)rtguest::G(0x335700)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar3)));
  }
  else if (bVar1 == 1) {
    (*(uint32_t*)rtguest::G(0x335700)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x017802d8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) * 0xc)));
  }
  else if ((bVar1 & 2) != 0) {
    (*(uint32_t*)rtguest::G(0x335700)) = 0;
  }
  puVar2 = 0x01786ff0;
  iVar3 = 5;
  do {
    iVar3 = iVar3 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0x7fffffff;
    puVar2 = puVar2 + 0x4;  // T* element step
  } while (-1 < iVar3);
  return 0;
}


RT_GUEST_FN(0x0022d288, guest::ResetRecordTables)

}  // namespace guest
