// guest:0x002829e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sinit(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = (int)param_1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c))) = 0x2829a8;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38))) = 1;
  RT_CALLRA(0x282a20, guest::std(iVar1 + 0x1e4,4,0,param_1));
  RT_CALLRA(0x282a34, guest::std(iVar1 + 0x23c,9,1,param_1));
  RT_CALLRA(0x282a48, guest::std(iVar1 + 0x294,10,2,param_1));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1d8))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1e0))) = iVar1 + 0x1e4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1dc))) = 3;
  return 0;
}


RT_GUEST_FN(0x002829e8, guest::__sinit)

}  // namespace guest
