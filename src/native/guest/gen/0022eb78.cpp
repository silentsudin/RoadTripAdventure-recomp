// guest:0x0022eb78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateZoneCheckFrame(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  ushort uVar2;
  undefined4 uVar3;
  uint uVar4;
uint32_t puVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = (int)param_2;
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22)));
  iVar6 = (int)param_1;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) & 0x44) == 4) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) + 1;
  }
  iVar8 = 0x17;
  puVar5 = 0x177ac50;
  do {
    if ((((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) != 0) && (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x33))) & 0x200) == 0)) &&
       (UpdateZoneFromRects(param_1,puVar5,param_2,cVar1 * 0x30 + 0x2a8198),
       (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + 0x19a))) != '\0')) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x4c))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x4c))) + 1;
    }
    iVar8 = iVar8 + -1;
    puVar5 = puVar5 + 0x4e;
  } while (-1 < iVar8);
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 9) == 9) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) | 8;
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) & 9) != 9) {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
      goto LAB_0022ec64;
    }
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  }
  else {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x30)));
LAB_0022ec64:
    if ((uVar2 & 0x800) == 0) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
      goto LAB_0022ec94;
    }
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xc))) = (*(uint8_t*)rtguest::G(0x22ee00));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28))) = uVar4 | 8;
  PlayBgmSlot5();
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
LAB_0022ec94:
  raceHudDraw(uVar3);
  return 0;
}


RT_GUEST_FN(0x0022eb78, guest::UpdateZoneCheckFrame)

}  // namespace guest
