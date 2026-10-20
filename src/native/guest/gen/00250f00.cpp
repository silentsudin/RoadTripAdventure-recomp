// guest:0x00250f00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitMiniGameBallState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  float fVar1;
  int iVar2;
  
  RT_CALLRA(0x250f24, guest::InitCameraTransforms(param_2,0x177ac50));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) * 0x40;
  RT_CALLRA(0x250f5c, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0b0))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0b4))),
             (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0b8))),0,(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0e0)))));
  (*(uint32_t*)rtguest::G(0x177ae8c)) = 0x40000;
  (*(uint8_t*)rtguest::G(0x177ae97)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(param_1 + 0x40))), 0, 0x60));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x58))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x44))) = 0x3f800000;
  fVar1 = (*(uint32_t*)rtguest::G(0x332fb0));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) * 0x40;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x60))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ac0c0 + iVar2)));
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 100))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0c4))) + fVar1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x68))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2ac0c8)));
  return 0;
}


RT_GUEST_FN(0x00250f00, guest::InitMiniGameBallState)

}  // namespace guest
