// guest:0x00223760 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SelectCarAttachmentPoint(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_20 = rtguest::allocFrame(4);
    uint32_t uStack_1c = rtguest::allocFrame(4);
    uint32_t uStack_18 = rtguest::allocFrame(4);
    uint32_t uStack_14 = rtguest::allocFrame(4);
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20)) = 0;
  switch(param_3) {
  case 0:
  case 1:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0xc)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8)));
    break;
  case 2:
  case 3:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x14)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x10)));
    break;
  case 4:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x1c)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x18)));
    break;
  case 5:
  case 9:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x24)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x20)));
    break;
  case 6:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x2c)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x28)));
    break;
  case 8:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_18)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x34)));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_1c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x30)));
  }
  vu0ApplyMatrix(param_1,param_2,uStack_20);
  return 0;
}


RT_GUEST_FN(0x00223760, guest::SelectCarAttachmentPoint)

}  // namespace guest
