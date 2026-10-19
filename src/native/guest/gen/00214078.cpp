// guest:0x00214078 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetControllerButtonConfig(uint32_t param_1, uint32_t param_2, ...) {
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x18)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c)));
  iVar4 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) + 0xf) * 4));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12e8))) =
       (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)))) * 4)) * 10) * 4));
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar2 + 0x1e) * 4));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ea))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar4 * 10) * 4));
  iVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar3 + 0x2d) * 4));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ec))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar2 * 10) * 4));
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar5 + 0x3c) * 4));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ee))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar3 * 10) * 4));
  iVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar4 + 0x4b) * 4));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x2c)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f6))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar2 * 10) * 4));
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar5 + 0x5a) * 4));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar3 * 10) * 4));
  iVar3 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar4 + 0x69) * 4));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f4))) = uVar1;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f8))) = uVar1;
  iVar4 = (*(uint32_t*)rtguest::G((uint32_t)0x29c620 + (uint32_t)(iVar5 + 0x78) * 4));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar2 * 10) * 4));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f2))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar3 * 10) * 4));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12fa))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x29c538 + (uint32_t)(iVar4 * 10) * 4));
  return 0;
}


RT_GUEST_FN(0x00214078, guest::SetControllerButtonConfig)

}  // namespace guest
