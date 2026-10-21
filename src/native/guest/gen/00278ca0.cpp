// guest:0x00278ca0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdDiskReady(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::gS[6] = 0x00000000002e0000ull;  // walk const
  }
  int iVar1;
  undefined4 uVar2;
  long lVar3;
  
  if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
    RT_CALLRA(0x278cdc, guest::scePrintf(0x331d08));
  }
  RT_CALLRA(0x278ce4, guest::cmd_sem_init());
  iVar1 = RT_CALLRA(0x278cf0, guest::PollSema((*(int32_t*)rtguest::G(0x2e6f2c))));
  uVar2 = 6;
  if ((*(int32_t*)rtguest::G(0x2e6f2c)) == iVar1) {
    lVar3 = RT_CALLRA(0x278d04, guest::sceCdSyncS(1));
    if (lVar3 == 0) {
      RT_CALLRA(0x278d14, guest::sceSifInitRpc(0));
      if ((*(int32_t*)rtguest::G(0x2e6f44)) < 0) {
        while( true ) {
          while (lVar3 = sceSifBindRpc(0x17737d0,0xffffffff8000059a,0), lVar3 < 0) {
            if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
              RT_CALLRA(0x278d8c, guest::scePrintf(0x331d18));
            }
            iVar1 = 0x100000;
            do {
              iVar1 = iVar1 + -1;
            } while (iVar1 != -1);
          }
          iVar1 = 0x100000;
          if ((*(uint32_t*)rtguest::G(0x17737f4)) != 0) break;
          do {
            iVar1 = iVar1 + -1;
          } while (iVar1 != -1);
        }
        (*(uint32_t*)rtguest::G(0x2e6f44)) = 0;
      }
      (*(uint32_t*)rtguest::G(0x1773810)) = (uint32_t)param_1;
      sceSifWriteBackDCache(0x1773810,4);
      lVar3 = RT_CALLRA(0x278e14, guest::sceSifCallRpc(0x17737d0,0,0,0x1773810,4,0x2e8100,4,0));
      if (-1 < lVar3) {
        if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
          RT_CALLRA(0x278e4c, guest::scePrintf(0x331d38));
        }
        RT_CALLRA(0x278e28, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
        return 0;
      }
    }
    RT_CALLRA(0x278e60, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
    uVar2 = 6;
    if (param_1 == 8) {
      uVar2 = 0xffffffff;
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x00278ca0, guest::sceCdDiskReady)

}  // namespace guest
