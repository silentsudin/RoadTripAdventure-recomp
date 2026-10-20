// guest:0x00213c40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetControllerButtonConfig(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
  ushort uVar2;
  int iVar3;
uint32_t psVar4;
uint32_t piVar5;
  int iVar6;
  int iVar7;
  
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12e8))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c620;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ea))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c65c;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ec))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c698;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x18))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12ee))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c6d4;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f6))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c710;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f4))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c74c;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x24))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f0))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c788;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = iVar6;
  psVar4 = 0x29c538;
  iVar7 = 0;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12f2))) != (long)sVar1)
  {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c7c4;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x12fa)));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x2c))) = iVar6;
  iVar7 = 0;
  psVar4 = 0x29c538;
  while (sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar4)), psVar4 = psVar4 + 10 * sizeof(int16_t), (uint32_t)uVar2 != (long)sVar1) {
    iVar7 = iVar7 + 1;
  }
  iVar6 = 0;
  piVar5 = 0x0029c800;
  while (iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar5)), piVar5 = piVar5 + sizeof(int32_t), iVar7 != iVar3) {
    iVar6 = iVar6 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) = iVar6;
  return 0;
}


RT_GUEST_FN(0x00213c40, guest::GetControllerButtonConfig)

}  // namespace guest
