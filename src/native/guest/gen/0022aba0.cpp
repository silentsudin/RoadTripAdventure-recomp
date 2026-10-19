// guest:0x0022aba0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceAnimationPhaseState(uint32_t param_1, ...) {
  char cVar1;
  short sVar2;
  
  sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
  if (sVar2 < 4) {
    if (sVar2 < 2) {
      if (sVar2 == 1) {
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
        if (cVar1 == '\r') {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0xc;
          PlayUiSound(0x35);
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
        }
        if ((uint32_t)(cVar1 - 4U) < 2) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 6;
          PlayUiSound(0x35);
          return 0;
        }
      }
    }
    else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) == '\r') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 2;
      vu0UnitMatrix(0x17525d0);
      vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f34c)),0x17525d0,0x17525d0);
      vu0CopyVectorXYZ(0x1752600,0x29f340);
      (*(uint8_t*)rtguest::G(0x175265c)) = (*(uint32_t*)rtguest::G(0x333688));
      (*(uint32_t*)rtguest::G(0x1752650)) = 0;
      (*(uint32_t*)rtguest::G(0x1752658)) = 0;
      (*(uint32_t*)rtguest::G(0x1752654)) = 0;
      vu0UnitMatrix(0x1752610);
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = sVar2 + -1;
    }
  }
  else if (sVar2 == 4) {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
    if (cVar1 == '\x05') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 6;
    }
    else {
      if (cVar1 == '\r') {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 9;
        PlayUiSound(0x34);
      }
      if (cVar1 == '\x0f') {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = 0x10;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = 0;
        PlayUiSound(0x34);
      }
    }
  }
}


RT_GUEST_FN(0x0022aba0, guest::AdvanceAnimationPhaseState)

}  // namespace guest
