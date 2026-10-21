// guest:0x00269e88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopMenuDispatchByContext(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
uint32_t pcVar1;
  long lVar2;
  int iVar3;
  
  lVar2 = RT_CALLRA(0x269ea0, guest::ClassifyObjectContext(param_2));
  iVar3 = (int)param_1;
  if (lVar2 == 2) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x269900;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) = 0;
  }
  else {
    if (lVar2 < 3) {
      if (lVar2 != 1) {
        pcVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc)));
        goto LAB_00269f28;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) = 0;
      pcVar1 = 0x269568;
    }
    else {
      if (lVar2 != 4) {
        pcVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc)));
        goto LAB_00269f28;
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) = 0;
      pcVar1 = 0x268e40;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = pcVar1;
  }
  pcVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc)));
LAB_00269f28:
  RT_CALLRA(0x00269f28, rtCall(pcVar1, param_1,param_2));
  return 0;
}


RT_GUEST_FN(0x00269e88, guest::shopMenuDispatchByContext)

}  // namespace guest
