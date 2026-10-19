// guest:0x00217c68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarTireSound(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  char cVar1;
  undefined2 uVar2;
  ushort uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  long lVar8;
uint32_t piVar9;
  int iVar10;
  
  uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x19c)));
  uVar7 = uVar4 & 0x700;
  iVar10 = 3;
  if (uVar7 != 0x200) {
    if (uVar7 < 0x201) {
      uVar5 = 0x100;
      iVar10 = 1;
    }
    else {
      uVar5 = 0x600;
      iVar10 = 2;
    }
    if (uVar7 != uVar5) {
      iVar10 = 0;
    }
  }
  if (param_3 < 0x41) {
    if (iVar10 == 0) {
LAB_00217d04:
      if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1bc))) != 0) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1f8))) & 0x40) != 0)) {
        iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208)));
        goto LAB_00217d38;
      }
    }
    else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1bc))) != 0) {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1e0))) != 0 || (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1e4))) != 0) {
        iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208)));
      }
      goto LAB_00217d04;
    }
    if (iVar10 == 2) {
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208)));
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x213))) == '\x01') goto LAB_00217d38;
    }
    else {
      iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208)));
    }
    iVar6 = -iVar10;
    if (iVar10 == 0) goto LAB_00217d4c;
  }
  else {
    iVar10 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208)));
LAB_00217d38:
    iVar6 = iVar10 + 1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x208))) = iVar6;
LAB_00217d4c:
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198)));
  if ((uVar3 & 3) != 0) {
    if ((uVar3 & 0xa00) != 0) {
      SoundStopVoiceByHandle(uVar3 & 1,0);
      SndRpcCmd22((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x198))) & 1,2,0,0,0);
      return 0;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0x40) == 0) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x213)));
      uVar7 = uVar3 & 1;
      iVar10 = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x29cba8u + ((int)uVar4 >> 3 & 0xeU)))) + uVar7;
      uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1d0)));
      if (param_3 < 8) {
        iVar10 = 0;
        lVar8 = 0;
      }
      else if (param_3 < 0x20) {
        lVar8 = 0x40;
      }
      else {
        lVar8 = param_3;
        if (param_3 < 0x60) {
          lVar8 = 0xc0;
        }
        if (param_3 < 0x40) {
          lVar8 = 0x80;
        }
      }
      piVar9 = 0x00335728 + uVar7;
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar9));
      if (0x80 < lVar8) {
        lVar8 = 0x100;
      }
      if (iVar6 != iVar10) {
        if (iVar6 != 0) {
          PlayUiSound(iVar6 + 0x8000);
        }
        if (iVar10 == 0) {
          (*(int32_t*)rtguest::G((uint32_t)piVar9)) = 0;
        }
        else {
          PlayUiSound(iVar10);
          (*(int32_t*)rtguest::G((uint32_t)piVar9)) = iVar10;
        }
      }
      SoundStopVoiceByHandle(uVar7,lVar8);
      param_4 = param_4 & 1;
      if ('\0' < cVar1) {
        param_4 = 0;
      }
      SndRpcCmd22(uVar7,2,param_4,uVar2,(int)uVar4 >> 0x10 & 1);
    }
  }
}


RT_GUEST_FN(0x00217c68, guest::UpdateCarTireSound)

}  // namespace guest
