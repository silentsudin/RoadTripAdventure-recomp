// guest:0x00258bf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmdCStruct(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  (*(uint64_t*)rtguest::G(0x175e880)) = (*(uint64_t*)rtguest::G((uint32_t)param_1));
  (*(uint64_t*)rtguest::G(0x175e888)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t)));
  RT_CALLRA(0x258c90, guest::sceSifCallRpc(0x175e840,0xc,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00258bf0, guest::SndRpcCmdCStruct)

}  // namespace guest
