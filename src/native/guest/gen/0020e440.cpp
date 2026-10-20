// guest:0x0020e440 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdReadPoll(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  int iVar1;
  int iVar2;
  long lVar3;
  
  if ((*(uint32_t*)rtguest::G(0x3356c4)) == 0) {
    lVar3 = sceCdDiskReady(1);
    if ((lVar3 == 2) &&
       (lVar3 = sceCdRead((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356cc)) + 4))),
                          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356cc)) + 8))),(*(uint32_t*)rtguest::G(0x3356c8)),0x333380), lVar3 != 0
       )) {
      (*(uint32_t*)rtguest::G(0x3356c4)) = 1;
    }
  }
  else {
    lVar3 = sceCdSync(1);
    if (lVar3 != 0) {
      return lVar3;
    }
    iVar1 = sceCdGetError();
    if (iVar1 == 0) {
      return 0;
    }
    if (iVar1 < 1) {
      iVar2 = -1;
    }
    else {
      iVar2 = 0x13;
    }
    if (iVar1 != iVar2) {
      (*(uint32_t*)rtguest::G(0x3356c4)) = 0;
    }
  }
  return 1;
}


RT_GUEST_FN(0x0020e440, guest::cdReadPoll)

}  // namespace guest
