// guest:0x0025b908 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEntryMoveCursor(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_20 = rtguest::allocFrame(8);
  bool bVar1;
  byte bVar2;
  ushort uVar3;
  ushort uVar4;
  ushort uVar5;
  ulong uVar6;
  ushort uVar7;
  
  bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  uVar7 = (uint32_t)bVar2;
  uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x2c3b14u + (uint32_t)bVar2 * 4)));
  (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 0)) = (uint32_t)uVar6;
  bVar1 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)uStack_20)) == 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = uVar6;
  if (bVar1) {
    (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 2)) = (uint32_t)(uVar6 >> 0x10);
    if ((*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 2)) != 0) {
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
      goto LAB_0025ba2c;
    }
    (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 4)) = (uint32_t)(uVar6 >> 0x20);
    if ((*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 4)) != 0) {
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
    }
    (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 6)) = (uint32_t)(uVar6 >> 0x30);
    if ((*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 6)) != 0) {
      uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
    }
    if (bVar2 / 0x1a == 0) {
      uVar4 = bVar2 + 0x4e | 0x8000;
    }
    else {
      uVar4 = bVar2 - 0x1a;
    }
    if (bVar2 / 0x1a == 3) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + 0)) = CONCAT22((uint32_t)bVar2 % 0x1a,uVar4) | 0x80000000;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_20 + 0)) = CONCAT22(bVar2 + 0x1a,uVar4);
    }
    if ((uint32_t)bVar2 % 0x1a == 0) {
      uVar4 = bVar2 + 0x19 | 0x8000;
    }
    else {
      uVar4 = bVar2 - 1;
    }
    if ((uint32_t)bVar2 % 0x1a == 0x19) {
      uVar5 = bVar2 - 0x19 | 0x8000;
    }
    else {
      uVar5 = bVar2 + 1;
    }
    (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = CONCAT26(uVar5,CONCAT24(uVar4,(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)uStack_20))));
  }
  uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
LAB_0025ba2c:
  uVar5 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)uStack_20));
  if ((((uVar4 & 0x1000) != 0) ||
      ((((uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34))), (uVar3 & 0x1000) != 0 && (((*(uint64_t*)rtguest::G((uint32_t)uStack_20)) & 0x8000) == 0))
        || (uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 2)), (uVar4 & 0x4000) != 0)) ||
       (((uVar3 & 0x4000) != 0 && (uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 2)), ((*(uint64_t*)rtguest::G((uint32_t)uStack_20)) & 0x80000000) == 0)))))) ||
     ((uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 4)), (uVar4 & 0x8000) != 0 ||
      ((((uVar3 & 0x8000) != 0 && (uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 4)), ((*(uint64_t*)rtguest::G((uint32_t)uStack_20)) & 0x800000000000) == 0)) ||
       ((uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 6)), (uVar4 & 0x2000) != 0 ||
        ((uVar4 = uVar7, (uVar3 & 0x2000) != 0 &&
         (uVar5 = (*(uint16_t*)rtguest::G((uint32_t)uStack_20 + 6)), ((*(uint64_t*)rtguest::G((uint32_t)uStack_20)) & 0x8000000000000000) == 0)))))))))) {
    uVar4 = uVar5 & 0x7fff;
  }
  if (uVar7 != uVar4) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (char)uVar4;
    PlayUiSound(0x1d);
  }
  return 0;
}


RT_GUEST_FN(0x0025b908, guest::textEntryMoveCursor)

}  // namespace guest
