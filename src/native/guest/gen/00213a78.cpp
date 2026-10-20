// guest:0x00213a78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleToggleMenuInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  ushort uVar1;
  int iVar2;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (iVar2 == 100) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) & 0x7f;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
LAB_00213b94:
    if ((uVar1 & 0x10) == 0) {
      return (*(int32_t*)rtguest::G((uint32_t)param_4));  // dropped *param_4 return
    }
    PlayUiSound(0x1b);
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) | 0x80;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 300;
    return 300;
  }
  if (100 < iVar2) {
    if (iVar2 == 200) {
      return 200;
    }
    if (iVar2 < 0xc9) {
      if (iVar2 != 0x65) {
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
        goto LAB_00213c10;
      }
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
      goto LAB_00213b94;
    }
    if (iVar2 == 300) {
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x12d;
    }
    else if (iVar2 != 0x12d) {
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
      goto LAB_00213c10;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + 0x28;
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + -1;
    if (iVar2 + -1 == 0) {
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 399;
      return 399;
    }
    goto LAB_00213c20;
  }
  if (iVar2 == 1) {
LAB_00213b30:
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 2;
  }
  else {
    if (iVar2 < 2) {
      if (iVar2 != 0) {
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
        goto LAB_00213c10;
      }
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0xe))) | 0x80;
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 1;
      goto LAB_00213b30;
    }
    if (iVar2 != 2) {
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
LAB_00213c10:
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      return 0;
    }
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + -1;
  if (iVar2 + -1 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 100;
    return 100;
  }
LAB_00213c20:
  return (*(int32_t*)rtguest::G((uint32_t)param_4));  // dropped *param_4 return
}


RT_GUEST_FN(0x00213a78, guest::HandleToggleMenuInput)

}  // namespace guest
