// guest:0x00223150 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UploadPackedColors(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
    uint32_t uStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
    uint32_t uStack_28 = rtguest::allocFrame(4);
    uint32_t uStack_24 = rtguest::allocFrame(4);
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
    uint32_t uStack_14 = rtguest::allocFrame(4);
  uint uVar1;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_30)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 & 0xf) * 4)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 >> 2 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 >> 6 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 >> 10 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 >> 0xe & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar1 >> 0x12 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_24)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14)) = 0;
  RT_CALLRA(0x2231e8, guest::vifUploadColours(uStack_30,uStack_20));
  return 0;
}


RT_GUEST_FN(0x00223150, guest::UploadPackedColors)

}  // namespace guest
