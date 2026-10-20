// guest:0x00271598 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridPutChar(uint64_t param_1, uint32_t param_2, uint32_t param_3) {
  rtguest::FrameGuard rt_fg;
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
    textBufferSetPixel16(param_1,(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))),param_2 | (param_3 & 0xff) << 8);
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
    textGridScrollUp(param_1,(uVar1 - (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t)))) + 1);
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) - 1;
  }
  return;
}


RT_GUEST_FN(0x00271598, guest::textGridPutChar)

}  // namespace guest
