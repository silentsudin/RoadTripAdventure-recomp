// guest:0x0026df30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void PrintVec2Short(uint32_t param_1, int param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::gS[0] = 0x000000000032fbd0ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
  }
  undefined8 uVar1;
  undefined8 uVar2;
  uint32_t auStack_70 = rtguest::allocFrame(64);

  uVar1 = RT_CALLRA(0x26df58, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1)))));
  uVar2 = RT_CALLRA(0x26df64, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 8)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x32fbd0)),
            uVar1, uVar2);
  RT_CALLRA(0x26df8c, guest::uiDebugText8(0x10, param_2 << 3, auStack_70, 0xf));
  return;
}


RT_GUEST_FN(0x0026df30, guest::PrintVec2Short)

}  // namespace guest
