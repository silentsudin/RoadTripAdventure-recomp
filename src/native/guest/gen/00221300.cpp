// guest:0x00221300 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t renderInit(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
uint32_t pcVar2;
uint32_t puVar3;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) = 0x2213e0;
  puVar3 = 0x177e6d0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) != 0) {
    puVar3 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 400 + 0x177e540);
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = puVar3;
  ResetCarEffectSlots();
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
  if (cVar1 == -1) {
    pcVar2 = 0x220938;
  }
  else if (cVar1 == '\"') {
    pcVar2 = 0x220b20;
  }
  else if (((cVar1 == '\x12') || (cVar1 == '\x1c')) || (cVar1 == ' ')) {
    pcVar2 = 0x220fe8;
  }
  else {
    pcVar2 = 0x220d40;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = pcVar2;
  return 0;
}


RT_GUEST_FN(0x00221300, guest::renderInit)

}  // namespace guest
