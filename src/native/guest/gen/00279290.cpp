// guest:0x00279290 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSdRemoteInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001770000ull;  // walk const
    rtguest::gS[2] = 0x00000000017739c0ull;  // walk const
  }
  int iVar1;
  long lVar2;
  
  RT_CALLRA(0x2792b0, guest::sceSifInitRpc(0));
  do {
    lVar2 = RT_CALLRA(0x2792c4, guest::sceSifBindRpc(0x17739c0,0xffffffff80000701,0));
    iVar1 = 10000;
    if (lVar2 < 0) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x331d78u)));
      return 0xffffffff;
    }
    do {
      iVar1 = iVar1 + -1;
    } while (iVar1 != -1);
  } while ((*(int32_t*)rtguest::G(0x17739e4)) == 0);
  RT_CALLRA(0x279310, guest::FlushCache(0));
  return 0;
}


RT_GUEST_FN(0x00279290, guest::sceSdRemoteInit)

}  // namespace guest
