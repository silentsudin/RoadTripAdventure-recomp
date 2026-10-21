// guest:0x00288628 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fsAllocFd(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  
  RT_CALLRA(0x288638, guest::fsFdSemInit());
  RT_CALLRA(0x288640, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
  uVar1 = (*(uint32_t*)rtguest::G(0x2e9990));
  uVar3 = 0x1779980;
  iVar2 = (*(uint32_t*)rtguest::G(0x1779984));
  while( true ) {
    if (iVar2 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(uVar3 + 4))) = 0x10000000;
      RT_CALLRA(0x288674, guest::SignalSema(uVar1));
      return uVar3;
    }
    if (0x1779b7f < uVar3 + 0x10) break;
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(uVar3 + 0x14)));
    uVar3 = uVar3 + 0x10;
  }
  RT_CALLRA(0x288690, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
  return 0;
}


RT_GUEST_FN(0x00288628, guest::fsAllocFd)

}  // namespace guest
