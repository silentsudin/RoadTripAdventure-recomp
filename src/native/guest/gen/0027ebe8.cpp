// guest:0x0027ebe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _raise_r(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
uint32_t pcVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
uint32_t puVar5;
uint32_t puVar6;
  
  puVar6 = (uint32_t)param_1;
  if (param_2 < 0x20) {
    iVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x75) * sizeof(uint32_t)));
    if (iVar4 == 0) {
      lVar3 = RT_CALLRA(0x27ec24, guest::_init_signal_r());
      if (lVar3 != 0) {
        return 0xffffffffffffffff;
      }
      iVar4 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(0x75) * sizeof(uint32_t)));
    }
    puVar5 = (uint32_t)((int)param_2 * 4 + iVar4);
    pcVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar5));
    if (pcVar1 == (uint32_t)0x0) {
      uVar2 = RT_CALLRA(0x27ec4c, guest::_getpid_r(param_1));
      uVar2 = _kill_r(param_1,uVar2,param_2);
      return uVar2;
    }
    uVar2 = 0;
    if (pcVar1 != (uint32_t)0x1) {
      if (pcVar1 == (uint32_t)0xffffffff) {
        uVar2 = 1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0x16;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0;
        RT_CALLRA(0x0027eca0, rtCall(pcVar1, param_2));
        uVar2 = 0;
      }
    }
  }
  else {
    uVar2 = 0xffffffffffffffff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0x16;
  }
  return uVar2;
}


RT_GUEST_FN(0x0027ebe8, guest::_raise_r)

}  // namespace guest
