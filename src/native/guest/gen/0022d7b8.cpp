// guest:0x0022d7b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RankCarsByProgress(uint32_t param_1, ...) {
  byte bVar1;
  byte bVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
uint32_t puVar6;
uint32_t puVar7;
  int iVar8;
uint32_t puVar9;
uint32_t puVar10;
  
  puVar6 = 0x177ac50;
  iVar8 = 0x17;
  puVar9 = (uint32_t)0x0;
  do {
    puVar10 = puVar9;
    if (((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) & 0x200) == 0)) {
      puVar7 = (uint32_t)0x0;
      while( true ) {
        bVar3 = false;
        if (puVar10 != (uint32_t)0x0) {
          bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x19a)));
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x19a)));
          iVar5 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x19b))) - (uint32_t)(bVar1 < 2);
          iVar4 = (int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x19b))) - (uint32_t)(bVar2 < 2);
          bVar3 = iVar4 < iVar5;
          if (((iVar5 == iVar4) && (bVar3 = bVar2 < bVar1, bVar1 == bVar2)) &&
             (bVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x24b))) < (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x24b))),
             (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x24b))) == (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x24b))))) {
            bVar3 = (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x24c))) <= (*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x24c)));
          }
        }
        if (!bVar3) break;
        puVar10 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x264)));
        puVar7 = puVar10;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x264))) = puVar10;
      puVar10 = puVar6;
      if (puVar7 != (uint32_t)0x0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar7 + 0x264))) = puVar6;
        puVar10 = puVar9;
      }
    }
    iVar8 = iVar8 + -1;
    puVar6 = puVar6 + 0x270;  // T* element step
    puVar9 = puVar10;
  } while (-1 < iVar8);
  for (; puVar10 != (uint32_t)0x0; puVar10 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x264)))) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar10 + 0x247))) = param_1;
    param_1 = param_1 + '\x01';
  }
  return 0;
}


RT_GUEST_FN(0x0022d7b8, guest::RankCarsByProgress)

}  // namespace guest
