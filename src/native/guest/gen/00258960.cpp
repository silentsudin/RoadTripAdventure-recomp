// guest:0x00258960 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmd30(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  RT_CALLRA(0x25898c, guest::sceSifCallRpc(0x175e840,0x30,0,0,0,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00258960, guest::SndRpcCmd30)

}  // namespace guest
