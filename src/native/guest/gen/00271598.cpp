// guest:0x00271598 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridPutChar(uint64_t param_1, uint32_t param_2, uint32_t param_3) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
uint32_t puVar2;
  
  param_2 = param_2 & 0xff;
  puVar2 = (uint32_t)param_1;
  if (param_2 < 0x20) {
    if (param_2 == 10) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) + 1;
    }
    if (param_2 == 0xd) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
    }
  }
  else {
    RT_CALLRA(0x2715c4, guest::textBufferSetPixel16(param_1,(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))),param_2 | (param_3 & 0xff) << 8));
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) + 1;
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) < (*(uint32_t*)rtguest::G((uint32_t)puVar2))) {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t)));
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) + 1;
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t)));
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) <= uVar1) {
    RT_CALLRA(0x271644, guest::textGridScrollUp(param_1,(uVar1 - (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t)))) + 1));
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) - 1;
  }
  return;
}


RT_GUEST_FN(0x00271598, guest::textGridPutChar)

}  // namespace guest
