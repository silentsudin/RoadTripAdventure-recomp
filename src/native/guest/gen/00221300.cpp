// guest:0x00221300 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t renderInit(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  char cVar1;
uint32_t pcVar2;
uint32_t puVar3;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c))) = 0x2213e0;
  puVar3 = 0x177e6d0;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) != 0) {
    puVar3 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 400 + 0x177e540);
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = puVar3;
  RT_CALLRA(0x221350, guest::ResetCarEffectSlots());
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
