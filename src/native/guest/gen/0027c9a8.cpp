// guest:0x0027c9a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __deregister_frame_info(uint32_t param_1, ...) {
uint32_t __ptr;
  int iVar1;
uint32_t piVar2;
  
  WaitSema((*(uint32_t*)rtguest::G(0x2e99ac)));
  piVar2 = 0x01776160;
  iVar1 = (*(uint32_t*)rtguest::G(0x1776160));
  while( true ) {
    if (iVar1 == 0) {
      SignalSema((*(uint32_t*)rtguest::G(0x2e99ac)));
      iVar1 = abort();
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == param_1) break;
    piVar2 = (uint32_t)(iVar1 + 0x14);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  }
  __ptr = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc)));
  (*(int32_t*)rtguest::G((uint32_t)piVar2)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x14)));
  if ((__ptr != (uint32_t)0x0) && (__ptr != param_1)) {
    free(__ptr);
  }
  SignalSema((*(uint32_t*)rtguest::G(0x2e99ac)));
}


RT_GUEST_FN(0x0027c9a8, guest::__deregister_frame_info)

}  // namespace guest
