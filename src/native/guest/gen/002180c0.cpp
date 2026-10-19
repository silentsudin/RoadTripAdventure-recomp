// guest:0x002180c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadCarPerformanceSpecs(uint32_t param_1, ...) {
  byte bVar1;
  short sVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
uint32_t puVar6;
uint32_t puVar7;
uint32_t psVar8;
  int iVar9;
  
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x180)));
  iVar9 = 0;
  psVar8 = (uint32_t)(param_1 + 0x21c);
  do {
    iVar5 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 1))) * 0xe + iVar9;
    iVar9 = iVar9 + 1;
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2eca7cu + iVar5 * 2)));
    (*(int16_t*)rtguest::G((uint32_t)psVar8)) = sVar2;
    iVar5 = (int)sVar2;
    if (iVar5 < 0) {
      iVar5 = iVar5 + 0xf;
    }
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x198))) & 0x30) != 0) {
      (*(int16_t*)rtguest::G((uint32_t)psVar8)) = sVar2 + (short)(iVar5 >> 4);
    }
    psVar8 = psVar8 + sizeof(int16_t);
  } while (iVar9 < 6);
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x244))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x214))) =
       (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ecedc + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2))) * 0x18)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x240))) =
       (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002ecee0 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2))) * 0x18)));
  iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed204 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 3))) * 0x10)));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x218))) = iVar9;
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x198))) & 0x30) != 0) {
    iVar5 = iVar9;
    if (iVar9 < 0) {
      iVar5 = iVar9 + 3;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x218))) = iVar9 - (iVar5 >> 2);
  }
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002eca8a + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 1))) * 0x1c))) == 0) {
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 5)));
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x218))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x218))) + 5;
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 5)));
  }
  puVar7 = (uint32_t)(param_1 + 0x22c);
  iVar9 = 7;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x242))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed4f4 + (uint32_t)bVar1 * 0x10)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x200))) =
       0x002ed5dc + ((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 6))) * 0xc - (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 6)))) * 4;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x245))) =
       (*(uint32_t*)rtguest::G((uint32_t)0x2f00ac + (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd))) * 0x10) * 4)) + ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x198))) & 1);
  puVar6 = (uint32_t)(0x002ed334 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) * 0x1c);
  do {
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)puVar6));
    puVar6 = puVar6 + sizeof(uint16_t);
    iVar9 = iVar9 + -1;
    (*(uint16_t*)rtguest::G((uint32_t)puVar7)) = uVar3;
    puVar7 = puVar7 + sizeof(uint16_t);
  } while (-1 < iVar9);
  return 0;
}


RT_GUEST_FN(0x002180c0, guest::LoadCarPerformanceSpecs)

}  // namespace guest
