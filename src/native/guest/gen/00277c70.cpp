// guest:0x00277c70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdDelayThread(uint32_t param_1, ...) {
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
  undefined8 uVar1;
uint32_t auStack_50 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0;
  uVar1 = CreateSema(auStack_50);
  SetAlarm(param_1,0x277c48,uVar1);
  WaitSema(uVar1);
  DeleteSema(uVar1);
  return 0;
}


RT_GUEST_FN(0x00277c70, guest::sceCdDelayThread)

}  // namespace guest
