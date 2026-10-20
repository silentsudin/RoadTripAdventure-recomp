// guest:0x0023d058 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeSlotScore(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  uint uVar1;
  int iVar2;
  ulong uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  
  iVar4 = 10;
  uVar6 = 0;
  if ((*(int8_t*)rtguest::G((uint32_t)0x177fdd4 + (uint32_t)(param_1 * 0x3448) * 1)) == '\0') {
    iVar4 = 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)0x177fdd6 + (uint32_t)(param_1 * 0x3448) * 1)) != '\0') {
    iVar4 = iVar4 + 10;
  }
  do {
    uVar1 = uVar6 & 0x1f;
    uVar6 = uVar6 + 1;
    if (((int)(uint32_t)(uint32_t)(*(uint16_t*)rtguest::G((uint32_t)0x1780a3a + (uint32_t)(param_1 * 0x1a24) * 2)) >> uVar1 & 1U) == 0) {
      iVar4 = iVar4 + 2;
    }
  } while ((int)uVar6 < 10);
  iVar5 = 0;
  for (uVar3 = (*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)(param_1 * 0x689) * 8)); uVar3 != 0; uVar3 = uVar3 >> 1) {
    iVar5 = iVar5 + ((uint32_t)uVar3 & 1);
  }
  for (uVar3 = (*(int64_t*)rtguest::G((uint32_t)0x177fcb0 + (uint32_t)(param_1 * 0x689) * 8)); uVar3 != 0; uVar3 = uVar3 >> 1) {
    iVar5 = iVar5 + ((uint32_t)uVar3 & 1);
  }
  iVar7 = 0;
  iVar4 = iVar4 + (char)(*(int8_t*)rtguest::G((uint32_t)0x177fdb1 + (uint32_t)(param_1 * 0x3448) * 1)) * 0x14 + (100 - iVar5) * 2;
  do {
    switch((*(uint32_t*)rtguest::G((uint32_t)0x1780750 + (uint32_t)(param_1 * 0x3448 + iVar7) * 4))) {
    case 0:
      iVar4 = iVar4 + 8;
      break;
    case 1:
    case 2:
      iVar4 = iVar4 + 4;
      break;
    case 3:
      iVar4 = iVar4 + 3;
      break;
    case 4:
      iVar4 = iVar4 + 2;
      break;
    case 5:
      iVar4 = iVar4 + 1;
    }
    iVar7 = iVar7 + 1;
  } while (iVar7 < 0x19);
  iVar5 = 0;
  for (uVar3 = (*(uint64_t*)rtguest::G((uint32_t)0x177fc78 + (uint32_t)(param_1 * 0x689) * 8)); uVar3 != 0; uVar3 = uVar3 >> 1) {
    iVar5 = iVar5 + ((uint32_t)uVar3 & 1);
  }
  for (uVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x177fc80 + (uint32_t)(param_1 * 0x689) * 4)); uVar3 != 0; uVar3 = uVar3 >> 1) {
    iVar5 = iVar5 + ((uint32_t)uVar3 & 1);
  }
  iVar7 = 0;
  for (uVar3 = (*(uint64_t*)rtguest::G((uint32_t)0x177fc68 + (uint32_t)(param_1 * 0x689) * 8)); uVar3 != 0; uVar3 = uVar3 >> 1) {
    iVar7 = iVar7 + ((uint32_t)uVar3 & 1);
  }
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x177fc70 + (uint32_t)(param_1 * 0x689) * 4));
  iVar2 = iVar7 << 1;
  if (uVar3 != 0) {
    do {
      uVar6 = (uint32_t)uVar3;
      uVar3 = uVar3 >> 1;
      iVar7 = iVar7 + (uVar6 & 1);
    } while (uVar3 != 0);
    iVar2 = iVar7 * 2;
  }
  return iVar4 + iVar5 * 2 + iVar2;
}


RT_GUEST_FN(0x0023d058, guest::ComputeSlotScore)

}  // namespace guest
