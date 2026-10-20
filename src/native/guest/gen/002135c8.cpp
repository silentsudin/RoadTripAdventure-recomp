// guest:0x002135c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleConfirmationMenuInput(uint64_t param_1, uint64_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  ushort uVar1;
  int iVar2;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (iVar2 == 0x65) {
LAB_002136b0:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
    if ((uVar1 & 0x40) == 0) {
      if ((uVar1 & 0x10) == 0) {
        iVar2 = NavigateGridSelection(param_2,(*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))),1,3,0,1);
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) = iVar2;
      }
      else {
        PlayUiSound(0x1b);
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 300;
      }
    }
    else {
      PlayUiSound(0x1a);
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 200;
    }
    goto LAB_0021379c;
  }
  if (iVar2 < 0x66) {
    if (iVar2 != 1) {
      if (iVar2 < 2) {
        if (iVar2 == 0) {
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 1;
          goto LAB_00213670;
        }
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      }
      else {
        if (iVar2 == 2) goto LAB_00213680;
        if (iVar2 == 100) {
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
          goto LAB_002136b0;
        }
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
      }
LAB_00213798:
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
      goto LAB_0021379c;
    }
LAB_00213670:
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 2;
LAB_00213680:
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + -1;
    if (iVar2 + -1 != 0) goto LAB_0021379c;
    iVar2 = 100;
  }
  else {
    if (iVar2 != 0xc9) {
      if (0xc9 < iVar2) {
        if (iVar2 == 300) {
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x12d;
        }
        else if (iVar2 != 0x12d) {
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
          goto LAB_00213798;
        }
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + 0x28;
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + -1;
        if (iVar2 + -1 != 0) goto LAB_0021379c;
        iVar2 = 399;
        goto LAB_00213788;
      }
      if (iVar2 != 200) {
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
        goto LAB_00213798;
      }
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0xc9;
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar2 + -1;
    if (iVar2 + -1 != 0) goto LAB_0021379c;
    iVar2 = 299;
  }
LAB_00213788:
  (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar2;
LAB_0021379c:
  return (*(int32_t*)rtguest::G((uint32_t)param_4));
}


RT_GUEST_FN(0x002135c8, guest::HandleConfirmationMenuInput)

}  // namespace guest
