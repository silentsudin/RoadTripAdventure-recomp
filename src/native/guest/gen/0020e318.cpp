// guest:0x0020e318 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cdReadEntryWait(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
  bool bVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x3356c8)) = (uint32_t)param_2;
  (*(uint32_t*)rtguest::G(0x3356cc)) = param_1;
  lVar2 = RT_CALLRA(0x20e350, guest::sceCdRead((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2,0x333380));
  (*(uint32_t*)rtguest::G(0x3356c4)) = (uint32_t)(lVar2 != 0);
LAB_0020e380:
  do {
    RT_CALLRA(0x20e380, guest::sceGsSyncV(0));
    if ((*(uint32_t*)rtguest::G(0x3356c4)) == 0) {
      lVar2 = RT_CALLRA(0x20e394, guest::sceCdDiskReady(1));
      bVar1 = true;
      if (lVar2 == 2) {
        lVar2 = RT_CALLRA(0x20e3b4, guest::sceCdRead((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356cc)) + 4))),
                          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356cc)) + 8))),(*(uint32_t*)rtguest::G(0x3356c8)),0x333380));
        if (lVar2 != 0) {
          (*(uint32_t*)rtguest::G(0x3356c4)) = 1;
        }
LAB_0020e418:
        bVar1 = true;
      }
    }
    else {
      lVar2 = RT_CALLRA(0x20e3d0, guest::sceCdSync(1));
      if (lVar2 != 0) goto LAB_0020e380;
      lVar2 = RT_CALLRA(0x20e3e4, guest::sceCdGetError());
      bVar1 = false;
      if (lVar2 != 0) {
        if (0 < lVar2) {
          if (lVar2 != 0x13) {
            (*(uint32_t*)rtguest::G(0x3356c4)) = 0;
          }
          goto LAB_0020e418;
        }
        bVar1 = true;
        if (lVar2 != -1) {
          (*(uint32_t*)rtguest::G(0x3356c4)) = 0;
        }
      }
    }
    if (!bVar1) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x0020e318, guest::cdReadEntryWait)

}  // namespace guest
