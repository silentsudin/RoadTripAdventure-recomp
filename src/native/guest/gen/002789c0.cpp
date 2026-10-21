// guest:0x002789c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdInit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  long lVar2;
  undefined4 uVar3;
  
  lVar2 = RT_CALLRA(0x2789f0, guest::sceCdSyncS(1));
  uVar3 = 0;
  if (lVar2 == 0) {
    RT_CALLRA(0x278a04, guest::sceSifInitRpc(0));
    (*(uint32_t*)rtguest::G(0x1773590)) = RT_CALLRA(0x278a0c, guest::GetThreadId());
    (*(uint32_t*)rtguest::G(0x2e6f24)) = 1;
    (*(int32_t*)rtguest::G(0x2e6f50)) = (*(int32_t*)rtguest::G(0x2e6f50)) + 1;
    (*(int32_t*)rtguest::G(0x2e6f3c)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f40)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f38)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f48)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f44)) = 0xffffffff;
    (*(uint32_t*)rtguest::G(0x2e6f34)) = 0;
    (*(uint32_t*)rtguest::G(0x2e6f4c)) = 0xffffffff;
    while( true ) {
      while (lVar2 = sceSifBindRpc(0x17737a8,0xffffffff80000592,0), lVar2 < 0) {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          RT_CALLRA(0x278ab4, guest::scePrintf(0x331cd8,lVar2,(*(int32_t*)rtguest::G(0x2e6f50))));
        }
        iVar1 = 0x100000;
        do {
          iVar1 = iVar1 + -1;
        } while (iVar1 != -1);
      }
      if ((*(uint32_t*)rtguest::G(0x17737cc)) != 0) break;
      iVar1 = 0x100000;
      do {
        iVar1 = iVar1 + -1;
      } while (iVar1 != -1);
    }
    (*(uint32_t*)rtguest::G(0x2e6f4c)) = 0;
    (*(uint32_t*)rtguest::G(0x1773800)) = param_1;
    sceSifWriteBackDCache(0x1773800,4);
    lVar2 = RT_CALLRA(0x278b28, guest::sceSifCallRpc(0x17737a8,0,0,0x1773800,4,0x2e8100,0x10,0));
    if (lVar2 < 0) {
      (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
      uVar3 = 0;
    }
    else {
      uVar3 = 2;
      (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
      if (((param_1 < 0) || (param_1 < 2)) || (param_1 != 5)) {
        RT_CALLRA(0x278c5c, guest::cmd_sem_init());
        RT_CALLRA(0x278c64, guest::PowerOffCB());
      }
      else {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          RT_CALLRA(0x278c28, guest::scePrintf(0x331cf8));
        }
        RT_CALLRA(0x278c30, guest::cdvd_exit());
        (*(int32_t*)rtguest::G(0x2e6f28)) = 0xffffffff;
        (*(int32_t*)rtguest::G(0x2e6f2c)) = 0xffffffff;
        (*(uint32_t*)rtguest::G(0x2e6f20)) = 0xffffffff;
      }
    }
  }
  return uVar3;
}


RT_GUEST_FN(0x002789c0, guest::sceCdInit)

}  // namespace guest
