// guest:0x0028a6a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDopen(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  int iVar1;
  long lVar2;
uint32_t piVar3;
  
  RT_CALLRA(0x28a6b8, guest::_sceFsWaitS(9));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28a6d0, guest::sceFsInit());
  }
  RT_CALLRA(0x28a6d8, guest::_sceFsSigSema());
  lVar2 = RT_CALLRA(0x28a6e0, guest::fsAllocFd());
  if (lVar2 == 0) {
    iVar1 = -0x13;
  }
  else {
    iVar1 = RT_CALLRA(0x28a6fc, guest::_sceCallCode(param_1,9));
    piVar3 = (uint32_t)lVar2;
    if (iVar1 < 0) {
      RT_CALLRA(0x28a710, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
      (*(int32_t*)rtguest::G((uint32_t)piVar3 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    }
    else {
      RT_CALLRA(0x28a724, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
      (*(int32_t*)rtguest::G((uint32_t)piVar3)) = iVar1;
      iVar1 = (int)(piVar3 + -0x5de660) >> 4;
    }
    RT_CALLRA(0x28a744, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
  }
  return iVar1;
}


RT_GUEST_FN(0x0028a6a0, guest::sceDopen)

}  // namespace guest
