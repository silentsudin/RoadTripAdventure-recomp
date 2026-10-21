// guest:0x00271668 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t textGridReadCellAdvance(uint64_t param_1) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
  undefined8 uVar2;
uint32_t puVar3;
  
  puVar3 = (uint32_t)param_1;
  uVar2 = RT_CALLRA(0x27167c, guest::textBufferGetPixel16(param_1,(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))),(*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t)))));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = uVar1 + 1;
  if ((*(uint32_t*)rtguest::G((uint32_t)puVar3)) <= uVar1 + 1) {
    if ((*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) < (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)))) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint32_t)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar3));
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00271668, guest::textGridReadCellAdvance)

}  // namespace guest
