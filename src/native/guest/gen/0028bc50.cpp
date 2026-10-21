// guest:0x0028bc50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInitIopHeap(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::gS[1] = 0x0000000001780000ull;  // walk const
  }
  int iVar1;
  long lVar2;
  
  while( true ) {
    lVar2 = RT_CALLRA(0x28bc9c, guest::sceSifBindRpc(0x1779c40,0xffffffff80000003,0));
    if (lVar2 < 0) {
      return 0xffffffff;
    }
    if ((*(uint32_t*)rtguest::G(0x1779c64)) != 0) break;
    iVar1 = 0x100000;
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  }
  (*(uint32_t*)rtguest::G(0x2e999c)) = 0;
  return 0;
}


RT_GUEST_FN(0x0028bc50, guest::sceSifInitIopHeap)

}  // namespace guest
