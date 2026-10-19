// guest:0x00285d28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t threadRequestLoop(uint32_t param_1, ...) {
  byte bVar1;
  int iVar2;
uint32_t puVar3;
  
  do {
    while( true ) {
      while( true ) {
        WaitSema((*(uint32_t*)rtguest::G(0x1776570)));
        iVar2 = ((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x1ff) * 2;
        (*(uint32_t*)rtguest::G((uint32_t)param_1)) = ((*(uint32_t*)rtguest::G((uint32_t)param_1)) & 0x1ff) + 1;
        puVar3 = (uint32_t)((int)param_1 + iVar2 + 9);
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + iVar2 + 8)));
        if (bVar1 != 1) break;
        RotateThreadReadyQueue((*(uint8_t*)rtguest::G((uint32_t)puVar3)));
      }
      if (1 < bVar1) break;
      if (bVar1 == 0) {
        WakeupThread((*(uint8_t*)rtguest::G((uint32_t)puVar3)));
      }
      else {
LAB_00285df0:
        ::printf((char *)rtguest::G((uint32_t)(0x332548)));
      }
    }
    if (bVar1 != 2) goto LAB_00285df0;
    SuspendThread((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + iVar2 + 9))));
  } while( true );
}


RT_GUEST_FN(0x00285d28, guest::threadRequestLoop)

}  // namespace guest
