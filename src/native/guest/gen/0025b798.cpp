// guest:0x0025b798 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEntrySelectInput(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  short sVar2;
  
  if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x10) == 0) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x40) == 0) {
      return 0;
    }
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) != 'Z') {
      if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) == '[') {
        if (0 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) {
          PlayUiSound(0x1a);
          return 1;
        }
        PlayUiSound(0x1c);
        return 0;
      }
      if ((long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) <
          (long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c40d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc)))) {
        PlayUiSound(0x22);
        sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + 1;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c40d8 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) * 0xc)));
        (*(uint32_t*)rtguest::G((uint32_t)0x1760880 + (uint32_t)((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) * 4)) =
             (*(uint32_t*)rtguest::G((uint32_t)(0x2c3b10u + (uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) * 4) * 4)));
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = sVar2;
        if (sVar2 != iVar1) {
          return 0;
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 0x5b;
        return 0;
      }
      goto LAB_0025b8e8;
    }
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
  }
  else {
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
  }
  if (0 < sVar2) {
    PlayUiSound(0x1b);
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) + -1;
    return 0;
  }
LAB_0025b8e8:
  PlayUiSound(0x1c);
  return 0;
}


RT_GUEST_FN(0x0025b798, guest::textEntrySelectInput)

}  // namespace guest
