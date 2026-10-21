// guest:0x0026de80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
// Widened arity (param_2 named) vs the one-arg decl: rename the stale decl.
#include "guest_decls.inc"

namespace guest {

uint32_t DrawMapCoordinateValues(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x68, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x000000000032fbb8ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
  }
  uint64_t uVar1;
  uint64_t uVar2;
  uint64_t uVar3;
  uint64_t uVar4;
  uint32_t auStack_80 = rtguest::allocFrame(64);

  uVar1 = RT_CALLRA(0x26deb0, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1)))));
  uVar2 = RT_CALLRA(0x26debc, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(1) * sizeof(uint32_t)))))));
  uVar3 = RT_CALLRA(0x26dec8, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(2) * sizeof(uint32_t)))))));
  uVar4 = RT_CALLRA(0x26ded4, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)((uint32_t)(param_1 + (uint32_t)(3) * sizeof(uint32_t)))))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x32fbb8)), uVar1, uVar2, uVar3, uVar4);
  RT_CALLRA(0x26df04, guest::uiDebugText8(0x10, param_2 << 3, auStack_80, 0xf));
  return 0;
}


RT_GUEST_FN(0x0026de80, guest::DrawMapCoordinateValues)

}  // namespace guest
