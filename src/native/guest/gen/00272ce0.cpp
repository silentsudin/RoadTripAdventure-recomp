// guest:0x00272ce0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t vu0RotMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  RT_CALLRA(0x272cf8, guest::vu0RotMatrixZ(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 8))));
  RT_CALLRA(0x272d08, guest::vu0RotMatrixY(*(uint32_t*)rtguest::G((uint32_t)(param_3 + 4)), param_1, param_1));
  vu0RotMatrixX(*(uint32_t*)rtguest::G((uint32_t)param_3), param_1, param_1);
  return 0;
}


RT_GUEST_FN(0x00272ce0, guest::vu0RotMatrix)

}  // namespace guest
