// guest:0x0026d700 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawRaceHudNumbers(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(96);
  uint uVar1;
  undefined8 uVar2;
  uint uVar3;
  uint uVar4;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = ComputeGridCellIndex((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xa0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xa8))));
  uVar4 = uVar1;
  if ((int)uVar1 < 0) {
    uVar4 = uVar1 + 3;
  }
  uVar3 = uVar1;
  if ((int)uVar1 < 0) {
    uVar3 = uVar1 + 0xf;
  }
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb18)), (int)uVar3 >> 4, (int)uVar4 >> 2 & 3, (int)(uVar1 & 2) >> 1 | (uVar1 & 1) << 1);
  uiDebugText8(0x20,0x18,auStack_60,0xf);
  uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335618)), uVar2);
  uiDebugText8(0x20,0x20,auStack_60,0xf);
  uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335620)), uVar2);
  uiDebugText8(0x20,0x28,auStack_60,0xf);
  uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x335628)), uVar2);
  uiDebugText8(0x20,0x30,auStack_60,0xf);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb28)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x19c))));
  uiDebugText8(0x20,0x38,auStack_60,0xf);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32fb38)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1f8))));
  uiDebugText8(0x20,0x40,auStack_60,0xf);
  return 0;
}


RT_GUEST_FN(0x0026d700, guest::DrawRaceHudNumbers)

}  // namespace guest
