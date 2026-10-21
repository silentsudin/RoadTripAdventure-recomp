// guest:0x00278f60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdRead(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::gS[5] = 0x00000000002e0000ull;  // walk const
  }
  byte bVar1;
  long lVar2;
  int iVar3;
  
  if ((((*(uint32_t*)rtguest::G(0x2e6f34)) & 1) == 0) && (lVar2 = sceCdNcmdDiskReady(), lVar2 == 6)) {
    return 0;
  }
  lVar2 = RT_CALLRA(0x278fbc, guest::_sceCd_ncmd_prechk(4));
  if (lVar2 != 0) {
    (*(uint32_t*)rtguest::G(0x2e7008)) = (uint32_t)param_3;
    (*(uint8_t*)rtguest::G(0x2e700c)) = (*(uint8_t*)rtguest::G((uint32_t)param_4));
    (*(uint8_t*)rtguest::G(0x2e700d)) = (*(uint8_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(uint8_t)));
    (*(uint8_t*)rtguest::G(0x2e700e)) = (*(uint8_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(uint8_t)));
    (*(uint32_t*)rtguest::G(0x2e7010)) = 0x002e8000;
    (*(uint32_t*)rtguest::G(0x2e7014)) = 0x002e80c0;
    bVar1 = (*(uint8_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(uint8_t)));
    if (bVar1 == 1) {
      iVar3 = param_2 * 0x918;
    }
    else {
      iVar3 = param_2 << 0xb;
      if ((1 < bVar1) && (bVar1 == 2)) {
        iVar3 = param_2 * 0x924;
      }
    }
    (*(uint32_t*)rtguest::G(0x2e80c0)) = 0;
    (*(uint32_t*)rtguest::G(0x2e7000)) = param_1;
    (*(uint32_t*)rtguest::G(0x2e7004)) = param_2;
    if (((*(uint32_t*)rtguest::G(0x2e6f34)) & 2) == 0) {
      sceSifWriteBackDCache(param_3,iVar3);
    }
    sceSifWriteBackDCache(0x2e8000,0x90);
    sceSifWriteBackDCache(0x2e7000,0x18);
    sceSifWriteBackDCache(0x2e80c0,4);
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      RT_CALLRA(0x279094, guest::scePrintf(0x331d50));
    }
    (*(int32_t*)rtguest::G(0x2e6f54)) = 1;
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 1;
    lVar2 = RT_CALLRA(0x2790d8, guest::sceSifCallRpc(0x2e80d0,1,1,0x2e7000,0x18,0,0,0x277f68));
    if (-1 < lVar2) {
      if ((*(uint32_t*)rtguest::G(0x002e6f10)) < 1) {
        return 1;
      }
      RT_CALLRA(0x279110, guest::scePrintf(0x331d68));
      return 1;
    }
    (*(int32_t*)rtguest::G(0x2e6f54)) = 0;
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
    RT_CALLRA(0x2790f4, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f28))));
  }
  return 0;
}


RT_GUEST_FN(0x00278f60, guest::sceCdRead)

}  // namespace guest
