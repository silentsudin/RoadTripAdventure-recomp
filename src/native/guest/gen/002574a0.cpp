// guest:0x002574a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcPlaySequence(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(192);
    rtguest::allocFrame(20);  // guest frame gap (sp+0x0..0x13)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0xb0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_1;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0xa8, 8);
    rtguest::gS[0] = 0x000000000175e880ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
  }
    uint32_t uStack_ac = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xac, frame 0xc0)
    rtguest::allocFrame(76);  // guest frame gap (sp+0x18..0x63)
    uint32_t uStack_5c = rtguest::allocFrame(4);  // sp+0x64 (Ghidra entry-sp -0x5c, frame 0xc0)
    uint32_t uStack_58 = rtguest::allocFrame(8);  // sp+0x68 (Ghidra entry-sp -0x58, frame 0xc0)
    uint32_t uStack_50 = rtguest::allocFrame(8);  // sp+0x70 (Ghidra entry-sp -0x50, frame 0xc0)
    uint32_t uStack_48 = rtguest::allocFrame(8);  // sp+0x78 (Ghidra entry-sp -0x48, frame 0xc0)
    uint32_t uStack_40 = rtguest::allocFrame(8);  // sp+0x80 (Ghidra entry-sp -0x40, frame 0xc0)
    uint32_t uStack_38 = rtguest::allocFrame(8);  // sp+0x88 (Ghidra entry-sp -0x38, frame 0xc0)
  
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)),param_1);
  RT_CALLRA(0x257534, guest::sceSifCallRpc(0x175e840,0x23,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = (uint32_t)param_1;
  (*(uint64_t*)rtguest::G(0x175e888)) = 1;
  (*(uint64_t*)rtguest::G(0x175e890)) = (uint64_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) << 0x20;
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  RT_CALLRA(0x2575b4, guest::sceSifCallRpc(0x175e840,0x22,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x002574a0, guest::SoundRpcPlaySequence)

}  // namespace guest
