// guest:0x0020f330 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ResetRaceRanks(uint32_t q0, ...) {
  byte bVar1;
uint32_t pbVar2;
uint32_t pbVar3;
  int iVar4;
  
  (*(uint8_t*)rtguest::G(0x1787012)) = 0xff;
  (*(uint8_t*)rtguest::G(0x178700a)) = 0xff;
  pbVar2 = 0x01787016;
  (*(uint32_t*)rtguest::G(0x178700e)) = 0xff;
  iVar4 = 3;
  pbVar3 = 0x0177b5e7;
  do {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
    pbVar3 = pbVar3 + 0x270;
    (*(int8_t*)rtguest::G((uint32_t)pbVar2)) = bVar1;
    if (bVar1 != 0xff) {
      (*(int8_t*)rtguest::G((uint32_t)pbVar2)) = bVar1 - (((*(uint8_t*)rtguest::G(0x177ae97)) < bVar1) + ((*(uint32_t*)rtguest::G(0x177b107)) < bVar1) + ((*(uint32_t*)rtguest::G(0x177b377)) < bVar1));
    }
    iVar4 = iVar4 + 1;
    pbVar2 = pbVar2 + 4;
  } while (iVar4 < 0x18);
  return 0;
}


RT_GUEST_FN(0x0020f330, guest::ResetRaceRanks)

}  // namespace guest
