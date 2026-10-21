// guest:0x00272ce0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t vu0RotMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
  }
  RT_CALLRA(0x272cf8, guest::vu0RotMatrixZ(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 8)), param_2, param_3));  // disasm: a1/a2 untouched entry->jal 0x272cf8; callee sqc2's 4x16B through o/i — 0 defaults wrote guest addr 0
  RT_CALLRA(0x272d08, guest::vu0RotMatrixY(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 4)), param_1, param_1));
  vu0RotMatrixX(*(uint32_t*)rtguest::G((uint32_t)param_3), param_1, param_1);
  return 0;
}


RT_GUEST_FN(0x00272ce0, guest::vu0RotMatrix)

}  // namespace guest
