// guest:0x00257fd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
typedef uint64_t rt_u64u __attribute__((aligned(1)));  // unaligned guest access: defined behavior, LE result unchanged

namespace guest {

uint32_t MusicRpcCmd15(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_1;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
  }
  uint32_t frame = rtguest::allocFrame(0x3c);
  uint32_t uStack_5c = frame;
  uint32_t uStack_58 = frame + 4;
  uint32_t uStack_50 = frame + 0xc;
  uint32_t uStack_48 = frame + 0x14;
  uint32_t uStack_40 = frame + 0x1c;
  uint32_t uStack_38 = frame + 0x24;
  uint32_t uStack_30 = frame + 0x2c;
  uint32_t uStack_28 = frame + 0x34;
  
  (*(uint64_t*)rtguest::G(0x175e888)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_58));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),param_1);
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_50));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_40));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_38));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_30));
  (*(uint64_t*)rtguest::G(0x175e8b8)) = (*(rt_u64u*)rtguest::G((uint32_t)uStack_28));
  RT_CALLRA(0x25805c, guest::sceSifCallRpc(0x175e840,0xf,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00257fd8, guest::MusicRpcCmd15)

}  // namespace guest
