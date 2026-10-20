// guest:0x0021f198 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyFreeCameraInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  ushort uVar1;
  char cVar2;
uint32_t pcVar3;
  int iVar4;
uint32_t puVar5;
  float fVar6;
  
  iVar4 = (int)param_1;
  pcVar3 = (uint32_t)(iVar4 + 0xc);
  puVar5 = (uint32_t)(pcVar3 + param_3 * 2 + 0x20);
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
  if ((uVar1 & 0x1000) != 0) {
    (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) + (*(float*)rtguest::G(0x332c5c));
  }
  if ((uVar1 & 0x4000) != 0) {
    (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(float))) - (*(uint32_t*)rtguest::G(0x332c60));
  }
  if ((uVar1 & 0x2000) != 0) {
    (*(float*)rtguest::G((uint32_t)param_2)) = (*(float*)rtguest::G((uint32_t)param_2)) + (*(uint32_t*)rtguest::G(0x332c64));
  }
  if ((uVar1 & 0x8000) != 0) {
    (*(float*)rtguest::G((uint32_t)param_2)) = (*(float*)rtguest::G((uint32_t)param_2)) - (*(uint32_t*)rtguest::G(0x332c68));
  }
  if ((uVar1 & 1) != 0) {
    (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float))) + (*(uint32_t*)rtguest::G(0x332c6c));
  }
  if ((uVar1 & 4) == 0) {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  else {
    (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(float))) - (*(uint32_t*)rtguest::G(0x332c70));
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar3));
  }
  if (cVar2 == '\a') {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x16))) =
         (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x16))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x3c))) * -0x80;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x3d))) * -0x80;
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd)));
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xd)));
  }
  if (cVar2 == '\a') {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x16))) =
         (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x16))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x40))) * -0x80;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x41))) * -0x80;
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
  }
  else {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
  }
  if ((uVar1 & 0x10) == 0) {
    fVar6 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(float)));
  }
  else {
    (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(float))) * (*(uint32_t*)rtguest::G(0x332c74));
    fVar6 = (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(float)));
  }
  if (500.0 < fVar6) {
    if ((uVar1 & 0x80) == 0) {
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb)));
    }
    else {
      (*(float*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(float))) = fVar6 / (*(uint32_t*)rtguest::G(0x332c78));
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb)));
    }
  }
  else {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb)));
  }
  if (cVar2 != '\0') {
    PrintDoorParams(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0021f198, guest::ApplyFreeCameraInput)

}  // namespace guest
