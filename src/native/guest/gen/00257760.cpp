// guest:0x00257760 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BindAndCallRpcInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[0] = 0x000000000175e840ull;  // walk const
  }
  RT_CALLRA(0x257780, guest::sceSifBindRpc(0x175e840,0x1234567,0));
  RT_CALLRA(0x2577a8, guest::sceSifCallRpc(0x175e840,0,0,0,0,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00257760, guest::BindAndCallRpcInit)

}  // namespace guest
