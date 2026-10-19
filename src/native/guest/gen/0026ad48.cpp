// guest:0x0026ad48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawAdjusterDebug(uint32_t param_1, ...) {
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335470)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb0))));
  uiDebugText8(0x10,0x18,auStack_40,0xf);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335478)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb2))));
  uiDebugText8(0x10,0x20,auStack_40,0xf);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335480)), (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xb4))));
  uiDebugText8(0x10,0x28,auStack_40,0xf);
  uVar1 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335488)), uVar1);
  uiDebugText8(0x10,0x30,auStack_40,0xf);
  uVar1 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335490)), uVar1);
  uiDebugText8(0x10,0x38,auStack_40,0xf);
  uVar1 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335498)), uVar1);
  uiDebugText8(0x10,0x40,auStack_40,0xf);
  return 0;
}


RT_GUEST_FN(0x0026ad48, guest::DrawAdjusterDebug)

}  // namespace guest
