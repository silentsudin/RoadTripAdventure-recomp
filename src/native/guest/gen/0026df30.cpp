// guest:0x0026df30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void PrintVec2Short(uint32_t param_1, int param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
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
