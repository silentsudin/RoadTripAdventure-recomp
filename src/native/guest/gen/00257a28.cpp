// guest:0x00257a28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SendMusicPlayRpc(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x68) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x6c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  uint32_t frame = rtguest::allocFrame(0x40);
  uint32_t uStack_60 = frame;
  uint32_t uStack_5c = frame + 4;
  uint32_t uStack_58 = frame + 8;
  uint32_t uStack_54 = frame + 0xc;
  uint32_t uStack_50 = frame + 0x10;
  uint32_t uStack_48 = frame + 0x18;
  uint32_t uStack_40 = frame + 0x20;
  uint32_t uStack_38 = frame + 0x28;
  uint32_t uStack_30 = frame + 0x30;
  uint32_t uStack_28 = frame + 0x38;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_60)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = param_1;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)uStack_54)), (char *)rtguest::G((uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_54)),(*(uint32_t*)rtguest::G((uint32_t)uStack_58)));
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
  (*(uint64_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_28));
  RT_CALLRA(0x257ae0, guest::sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00257a28, guest::SendMusicPlayRpc)

}  // namespace guest
