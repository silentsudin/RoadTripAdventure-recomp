// guest:0x002885c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fsFdSemInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::gS[0] = 0x00000000002f0000ull;  // walk const
  }
uint32_t auStack_40 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t uStack_38 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_2c = rtguest::allocFrame(4);
  
  if ((*(int32_t*)rtguest::G(0x2e9990)) == -1) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = 1;
    (*(int32_t*)rtguest::G(0x2e9990)) = RT_CALLRA(0x2885f8, guest::CreateSema(auStack_40));
    (*(uint32_t*)rtguest::G(0x2e9994)) = RT_CALLRA(0x288604, guest::CreateSema(auStack_40));
  }
  return 0;
}


RT_GUEST_FN(0x002885c8, guest::fsFdSemInit)

}  // namespace guest
