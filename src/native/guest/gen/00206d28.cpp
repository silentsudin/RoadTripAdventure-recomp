// guest:0x00206d28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t NavigateGridSelection(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint32_t param_4, uint64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
uint32_t puVar1;
  int iVar2;
  ushort uVar3;
  ushort uVar4;
  int iVar5;
  
  iVar2 = (int)param_3;
  if ((long)param_5 < 0) {
LAB_00206d9c:
    uVar4 = 0;
    uVar3 = 0;
  }
  else if (param_5 < 2) {
    puVar1 = (uint32_t)(param_1 + (int)param_5 * 2 + 0x30);
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(2) * sizeof(uint16_t)));
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)puVar1));
  }
  else {
    if (param_5 != 2) goto LAB_00206d9c;
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x36))) | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34)));
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x32))) | (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
  }
  iVar5 = param_2 / param_4;
  param_2 = param_2 % param_4;
  if (param_4 == 0) {
    trap(7);
  }
  if (param_6 == 0) {
LAB_00206e68:
    if ((uVar4 & 0x2000) != 0) {
      if (iVar5 < iVar2 + -1) {
        RT_CALLRA(0x206e58, guest::PlayUiSound(0x1d));
        iVar2 = (iVar5 + 1) * param_4;
      }
      else {
        iVar2 = iVar5 * param_4;
      }
      goto LAB_00206f0c;
    }
    if ((uVar4 & 0x8000) != 0) {
      if (iVar5 < 1) {
        iVar2 = iVar5 * param_4;
      }
      else {
        RT_CALLRA(0x206e84, guest::PlayUiSound(0x1d));
        iVar2 = (iVar5 + -1) * param_4;
      }
      goto LAB_00206f0c;
    }
    if ((uVar4 & 0x1000) != 0) {
      iVar2 = iVar5 * param_4;
      if (0 < param_2) {
        param_2 = param_2 + -1;
        RT_CALLRA(0x206eb0, guest::PlayUiSound(0x1d));
        iVar2 = iVar5 * param_4;
      }
      goto LAB_00206f0c;
    }
    if ((uVar4 & 0x4000) != 0) {
      iVar2 = iVar5 * param_4;
      if (param_4 + -1 <= param_2) goto LAB_00206f0c;
      param_2 = param_2 + 1;
      RT_CALLRA(0x206ed4, guest::PlayUiSound(0x1d));
    }
LAB_00206f08:
    iVar2 = iVar5 * param_4;
  }
  else {
    if ((uVar3 & 0x2000) == 0) {
      if ((uVar3 & 0x8000) != 0) {
        iVar5 = iVar5 + -1;
        if (iVar5 < 0) {
          iVar5 = iVar2 + -1;
        }
        goto joined_r0x00206e00;
      }
      if ((uVar3 & 0x1000) == 0) {
        if ((uVar3 & 0x4000) == 0) goto LAB_00206e68;
        param_2 = param_2 + 1;
        if (param_4 + -1 < param_2) {
          param_2 = 0;
        }
      }
      else {
        param_2 = param_2 + -1;
        if (param_2 < 0) {
          param_2 = param_4 + -1;
        }
      }
      if (param_4 == 1) goto LAB_00206f08;
    }
    else {
      iVar5 = iVar5 + 1;
      if (iVar2 + -1 < iVar5) {
        iVar5 = 0;
      }
joined_r0x00206e00:
      if (param_3 == 1) goto LAB_00206f08;
    }
    RT_CALLRA(0x206f00, guest::PlayUiSound(0x1d));
    iVar2 = iVar5 * param_4;
  }
LAB_00206f0c:
  return iVar2 + param_2;
}


RT_GUEST_FN(0x00206d28, guest::NavigateGridSelection)

}  // namespace guest
