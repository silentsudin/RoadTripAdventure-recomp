// guest:0x00258768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcCmd24(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(112);
    rtguest::allocFrame(20);  // guest frame gap (sp+0x0..0x13)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_1;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
  }
    uint32_t uStack_5c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x5c, frame 0x70)
    uint32_t uStack_58 = rtguest::allocFrame(8);  // sp+0x18 (Ghidra entry-sp -0x58, frame 0x70)
    uint32_t uStack_50 = rtguest::allocFrame(8);  // sp+0x20 (Ghidra entry-sp -0x50, frame 0x70)
    uint32_t uStack_48 = rtguest::allocFrame(8);  // sp+0x28 (Ghidra entry-sp -0x48, frame 0x70)
    uint32_t uStack_40 = rtguest::allocFrame(8);  // sp+0x30 (Ghidra entry-sp -0x40, frame 0x70)
    uint32_t uStack_38 = rtguest::allocFrame(8);  // sp+0x38 (Ghidra entry-sp -0x38, frame 0x70)
    uint32_t uStack_30 = rtguest::allocFrame(8);  // sp+0x40 (Ghidra entry-sp -0x30, frame 0x70)
    uint32_t uStack_28 = rtguest::allocFrame(8);  // sp+0x48 (Ghidra entry-sp -0x28, frame 0x70)
  
  (*(uint64_t*)rtguest::G(0x175e888)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),param_1);
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
  (*(uint64_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_28));
  RT_CALLRA(0x2587ec, guest::sceSifCallRpc(0x175e840,0x24,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00258768, guest::SndRpcCmd24)

}  // namespace guest
