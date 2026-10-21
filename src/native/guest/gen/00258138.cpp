// guest:0x00258138 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcReadBlock1F(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x000000000175f880ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  
  RT_CALLRA(0x258178, guest::sceSifCallRpc(0x175e840,0x1f,0,0,0,0x175f880,0x40,0));
  uVar3 = (*(uint64_t*)rtguest::G(0x175f898));
  uVar2 = (*(uint64_t*)rtguest::G(0x175f890));
  uVar1 = (*(uint64_t*)rtguest::G(0x175f888));
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x175f880));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar2;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar3;
  uVar3 = (*(uint64_t*)rtguest::G(0x175f8b8));
  uVar2 = (*(uint64_t*)rtguest::G(0x175f8b0));
  uVar1 = (*(uint64_t*)rtguest::G(0x175f8a8));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G(0x175f8a0));
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = uVar1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = uVar2;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = uVar3;
  return 0;
}


RT_GUEST_FN(0x00258138, guest::SndRpcReadBlock1F)

}  // namespace guest
