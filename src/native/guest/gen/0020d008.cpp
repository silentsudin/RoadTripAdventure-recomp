// guest:0x0020d008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleInputEvent(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  ushort uVar1;
  int iVar2;
  int iVar3;
  int in_v0_lo;
  uint uVar4;
  undefined8 uVar5;
  int unaff_s0_lo;
uint32_t puVar6;
uint32_t puVar7;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 4)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(unaff_s0_lo + 4)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f358 + param_1))) = 0x00497b80 + in_v0_lo;
  puVar7 = (uint32_t)(iVar2 + 0x100000);
  uVar4 = (uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)(iVar2) * 1));
  puVar6 = puVar7;
  if (uVar4 != param_3) {
    do {
      if (uVar4 == 0x10) {
        uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar6));
      }
      else {
        fatal(0x2ea490);
        uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar6));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x10 * sizeof(uint16_t)))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x10 * sizeof(uint16_t))));
      uVar4 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + (uint32_t)uVar1 * 0x10 + 0x13)));
      puVar6 = puVar6 + (uint32_t)uVar1 * 8 * sizeof(uint16_t) + 8 * sizeof(uint16_t);
    } while (uVar4 != 0x70);
  }
  uVar5 = sceDmaGetChan(1);
  sceDmaSync(uVar5,0,0);
  sendTexturePackets(puVar7);
  return iVar3 == -0x497b80;
}


RT_GUEST_FN(0x0020d008, guest::HandleInputEvent)

}  // namespace guest
