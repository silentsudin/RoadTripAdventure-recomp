// guest:0x0028a6a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDopen(uint32_t param_1, ...) {
  int iVar1;
  long lVar2;
uint32_t piVar3;
  
  _sceFsWaitS(9);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
  }
  _sceFsSigSema();
  lVar2 = fsAllocFd();
  if (lVar2 == 0) {
    iVar1 = -0x13;
  }
  else {
    iVar1 = _sceCallCode(param_1,9);
    piVar3 = (uint32_t)lVar2;
    if (iVar1 < 0) {
      WaitSema((*(uint32_t*)rtguest::G(0x2e9990)));
      (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    }
    else {
      WaitSema((*(uint32_t*)rtguest::G(0x2e9990)));
      (*(int32_t*)rtguest::G((uint32_t)piVar3)) = iVar1;
      iVar1 = (int)(piVar3 + -0x5de660) >> 4;
    }
    SignalSema((*(uint32_t*)rtguest::G(0x2e9990)));
  }
}


RT_GUEST_FN(0x0028a6a0, guest::sceDopen)

}  // namespace guest
