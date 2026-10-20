// guest:0x002230c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UploadPaintBgColors(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
    uint32_t uStack_14 = rtguest::allocFrame(4);
  
  param_1 = param_1 * 3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x29d1a0))) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x29d1a1))) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uint32_t)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x29d1a2 + (uint32_t)(param_1) * 1)) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14)) = 0;
  RT_CALLRA(0x223138, guest::vifUploadColours(uStack_20,uStack_20));
  return 0;
}


RT_GUEST_FN(0x002230c8, guest::UploadPaintBgColors)

}  // namespace guest
