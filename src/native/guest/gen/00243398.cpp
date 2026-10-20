// guest:0x00243398 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceMapPickups(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
  }
uint32_t afStack_70 = rtguest::allocFrame(40);
    uint32_t fStack_48 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_40 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_38 = rtguest::allocFrame(4);
uint32_t puVar1;
  float fVar2;
  
  fVar2 = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a4058 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x22))) * 0x10)));
  RT_CALLRA(0x2433e8, guest::vifUploadColours(0x2a43a0,0x2a4360));
  if (0 < param_2) {
    puVar1 = 0x0177eb40;
    do {
      param_2 = param_2 + -1;
      RT_CALLRA(0x243410, guest::vu0UnitMatrix(afStack_70));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) = (*(uint32_t*)rtguest::G((uint32_t)puVar1));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_38)) = (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(2) * sizeof(uint32_t)));
      puVar1 = puVar1 + 0x40;  // T* element step
      (*(float*)rtguest::G((uint32_t)afStack_70 + (uint32_t)(0) * 4)) = 50.0 / fVar2;
      (*(float*)rtguest::G((uint32_t)fStack_48)) = 50.0 / fVar2;
      RT_CALLRA(0x243434, guest::vifObjectMatrix(afStack_70));
      RT_CALLRA(0x24343c, guest::vifCallModel(0x290940));
    } while (param_2 != 0);
  }
  return 0;
}


RT_GUEST_FN(0x00243398, guest::raceMapPickups)

}  // namespace guest
