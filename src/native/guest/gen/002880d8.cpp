// guest:0x002880d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSetRpcQueue(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
uint32_t puVar2;
uint32_t puVar3;
  int iVar4;
  
  RT_CALLRA(0x2880ec, guest::DIntr());
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  puVar2 = (*(uint32_t*)rtguest::G(0x1778868));
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = 0;
  puVar3 = param_1;
  if (puVar2 != (uint32_t)0x0) {
    iVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t)));
    if ((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint32_t))) = param_1;
      puVar3 = (*(uint32_t*)rtguest::G(0x1778868));
    }
    else {
      do {
        iVar4 = iVar1;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)));
      } while (iVar1 != 0);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) = param_1;
      puVar3 = (*(uint32_t*)rtguest::G(0x1778868));
    }
  }
  (*(uint32_t*)rtguest::G(0x1778868)) = puVar3;
  EIntr();
  return 0;
}


RT_GUEST_FN(0x002880d8, guest::sceSifSetRpcQueue)

}  // namespace guest
