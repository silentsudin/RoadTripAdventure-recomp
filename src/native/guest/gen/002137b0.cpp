// guest:0x002137b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleAudioSettingsMenuInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  ushort uVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  iVar5 = (int)param_2;
  if (iVar4 == 100) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30)));
  }
  else {
    if (iVar4 < 0x65) {
      if (iVar4 == 1) {
LAB_0021388c:
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 2;
      }
      else {
        if (iVar4 < 2) {
          if (iVar4 != 0) {
            (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
LAB_00213a40:
            (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
            return 0;
          }
          ApplySoundVolumes((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))));
          sVar3 = (*(int16_t*)rtguest::G(0x1780a3e));
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) = (int)(*(int8_t*)rtguest::G(0x1780a3d));
          cVar2 = (*(int8_t*)rtguest::G(0x1780a3d));
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) = (int)sVar3;
          (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = (int)cVar2;
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 1;
          goto LAB_0021388c;
        }
        if (iVar4 != 2) {
          (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
          goto LAB_00213a40;
        }
      }
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + -0x28;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar4 + -1;
      if (iVar4 + -1 == 0) {
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 100;
        return 100;
      }
      goto LAB_00213a50;
    }
    if (iVar4 == 200) {
      return 200;
    }
    if (200 < iVar4) {
      if (iVar4 == 300) {
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x12d;
      }
      else if (iVar4 != 0x12d) {
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
      }
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x8210))) + 0x28;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t)));
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = iVar4 + -1;
      if (iVar4 + -1 == 0) {
        (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 399;
        goto LAB_00213a30;
      }
    }
    if (iVar4 != 0x65) {
      (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    }
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30)));
  }
  if ((uVar1 & 0x40) != 0) {
    PlayUiSound(0x1a);
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t)));
    if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) != iVar4) {
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) = iVar4;
      (*(int8_t*)rtguest::G(0x1780a3d)) = (char)iVar4;
      if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) != 0) {
        SoundRpcCmd33PlaySe((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(3) * sizeof(int32_t))) + -1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))));
        SoundRpcCmd34();
      }
LAB_00213a30:
      SoundRpcCmd35();

    }
LAB_00213a50:

  }
  if ((uVar1 & 0x10) != 0) {
    PlayUiSound(0x1b);
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 300;
    return 300;
  }
  if ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2c))) & 0x4000) == 0) || (iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) + -1, (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) < 1)) {
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x2c))) & 0x1000) == 0) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t)));
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) < 0x80) {
        iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) + 1;
        goto LAB_0021398c;
      }
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t)));
    }
    iVar4 = NavigateGridSelection(param_2,iVar4,3,1,0,1);
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(int32_t))) = iVar4;
  }
  else {
LAB_0021398c:
    (*(int16_t*)rtguest::G(0x1780a3e)) = (short)iVar4;
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) = iVar4;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(4) * sizeof(int32_t))) * 0x3fff;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0x7f;
  }
  SoundRpcCmd31(iVar4 >> 7,iVar4 >> 7);

}


RT_GUEST_FN(0x002137b0, guest::HandleAudioSettingsMenuInput)

}  // namespace guest
