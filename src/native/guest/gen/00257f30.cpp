// guest:0x00257f30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MusicRpcPlayBgm(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = (uint32_t)(int32_t)param_2;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
  }
  uint32_t frame = rtguest::allocFrame(0x38);
  uint32_t uStack_58 = frame;
  uint32_t uStack_50 = frame + 8;
  uint32_t uStack_48 = frame + 0x10;
  uint32_t uStack_40 = frame + 0x18;
  uint32_t uStack_38 = frame + 0x20;
  uint32_t uStack_30 = frame + 0x28;
  uint32_t uStack_28 = frame + 0x30;
  
  (*(uint64_t*)rtguest::G(0x175e888)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44(param_2,param_1);
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
  (*(uint64_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_28));
  RT_CALLRA(0x257fb8, guest::sceSifCallRpc(0x175e840,0xe,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00257f30, guest::MusicRpcPlayBgm)

}  // namespace guest
