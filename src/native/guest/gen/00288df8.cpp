// guest:0x00288df8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _fs_version(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[1] = 0x0000000001779ba8ull;  // walk const
    rtguest::gS[2] = 0x0000000000000000ull;  // walk const
    rtguest::gS[3] = 0x00000000002e9138ull;  // walk const
  }
  int iVar1;
  bool bVar2;
  
  bVar2 = false;
  iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x01779ba8)), (char *)rtguest::G((uint32_t)(0x2e9138u)), 4);
  if (iVar1 != 0) {
    iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x01779ba8)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x002e9998)))), 4);
    if (iVar1 != 0) {
      iVar1 = ::memcmp((char *)rtguest::G((uint32_t)(0x2e9138u)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x002e9998)))), 4);
      bVar2 = iVar1 != 0;
    }
  }
  return bVar2;
}


RT_GUEST_FN(0x00288df8, guest::_fs_version)

}  // namespace guest
