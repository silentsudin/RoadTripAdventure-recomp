// guest:0x00217f10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateContinuousSoundEffect(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  ushort uVar2;
  undefined4 uVar3;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0xc) != 4) {
    if (((*(uint16_t*)rtguest::G((uint32_t)param_2)) != 0) && ((param_5 & 3) != 0)) {
      PlayUiSound((param_5 & 1) + 0x8010);
    }
    (*(uint16_t*)rtguest::G((uint32_t)param_2)) = 0;
    return 0;
  }
  if (((param_3 & 8) == 0) || ((*(int32_t*)rtguest::G((uint32_t)param_4)) < 100)) {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0x40) == 0) {
      if ((*(uint16_t*)rtguest::G((uint32_t)param_2)) == 0) {
        return 0;
      }
      if ((param_5 & 3) != 0) {
        PlayUiSound((param_5 & 1) + 0x8010);
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
      }
    }
    if (0 < (short)uVar2) {
      (*(uint16_t*)rtguest::G((uint32_t)param_2)) = 0xfffe;
      return 0;
    }
    if (-1 < (short)uVar2) {
      return 0;
    }
    (*(uint16_t*)rtguest::G((uint32_t)param_2)) = uVar2 + 1;
    return 0;
  }
  (*(int32_t*)rtguest::G((uint32_t)param_4)) = (*(int32_t*)rtguest::G((uint32_t)param_4)) + -100;
  uVar3 = 0xb2;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0x40) == 0) {
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
    if ((*(uint16_t*)rtguest::G((uint32_t)param_2)) != 0) goto LAB_00217fa4;
    if ((param_5 & 3) != 0) {
      PlayUiSound((param_5 & 1) + 0x10);
      goto LAB_00217fa0;
    }
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  }
  else {
LAB_00217fa0:
    uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
LAB_00217fa4:
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  }
  uVar2 = uVar2 + 1;
  (*(uint16_t*)rtguest::G((uint32_t)param_2)) = uVar2;
  if (iVar1 < 12000) {
    uVar2 = uVar2 & 3;
    uVar3 = 0x2c;
  }
  else {
    if (29999 < iVar1) {
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
      goto LAB_00217fe4;
    }
    uVar2 = uVar2 & 7;
    uVar3 = 0x59;
  }
  (*(uint16_t*)rtguest::G((uint32_t)param_2)) = uVar2;
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)param_2));
LAB_00217fe4:
  if ((uVar2 == 0) && ((param_5 & 3) != 0)) {
    PlayUiSound((param_5 & 1) + 0x8010);
  }
  return uVar3;
}


RT_GUEST_FN(0x00217f10, guest::UpdateContinuousSoundEffect)

}  // namespace guest
