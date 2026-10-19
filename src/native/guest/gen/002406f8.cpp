// guest:0x002406f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateProximityEvent(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(4);
    uint32_t fStack_5c = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_54 = rtguest::allocFrame(4);
uint32_t auStack_50 = rtguest::allocFrame(4);
uint32_t auStack_4c = rtguest::allocFrame(12);
  byte bVar1;
  int iVar2;
  long lVar3;
  char cVar4;
  int iVar5;
  float fVar6;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = 0;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) | 1;
    SelectMeshIntersectionRoutine(0);
    (*(uint8_t*)rtguest::G(0x33573a)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = 1;
  }
  vu0CopyVector(auStack_60,0x2a2bc0);
  rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_60, auStack_50, auStack_4c, 3, param_2);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) == '\0') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) + 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = iVar2;
    if (0x3b < iVar2) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 1;
    }
  }
  else if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) == '\x01') &&
          (iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) + -1, (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = iVar2, iVar2 < 1)) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 0;
  }
  switch((*(uint8_t*)rtguest::G(0x33573a))) {
  case 0:
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x54))) != 0x2b) {
      return 0;
    }
    vu0SubVector(auStack_70,0x2a3870,0x177ace0);
    fVar6 = rtguest::fbits(vecDotProductVu0(auStack_70,auStack_70));
    if ((*(uint32_t*)rtguest::G(0x332ea8)) <= SQRT(fVar6)) {
      return 0;
    }
    fVar6 = rtguest::fbits(GetSpeedFromF15Vector(0x177ad40));
    if ((int)fVar6 < 10) {
      return 0;
    }
    PlayUiSound(0x14);
    (*(uint8_t*)rtguest::G(0x33573a)) = 1;
    (*(uint32_t*)rtguest::G(0x2a2bc8)) = (float)(*(uint32_t*)rtguest::G(0x332eb8));
    (*(uint32_t*)rtguest::G(0x33570c)) = (float)(*(uint32_t*)rtguest::G(0x332eac));
    (*(uint32_t*)rtguest::G(0x2a2bc0)) = (float)(*(uint32_t*)rtguest::G(0x332eb0));
    (*(uint32_t*)rtguest::G(0x2a2bc4)) = (float)(*(uint32_t*)rtguest::G(0x332eb4));
    switchD_002407e4_proximityEventNoOpCase();
    return 0;
  case 1:
    (*(uint32_t*)rtguest::G(0x2a2bc4)) = (*(uint32_t*)rtguest::G(0x2a2bc4)) - (*(uint32_t*)rtguest::G(0x33570c));
    (*(uint32_t*)rtguest::G(0x33570c)) = (*(uint32_t*)rtguest::G(0x33570c)) + (*(uint32_t*)rtguest::G(0x332ebc));
    (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) - (*(uint32_t*)rtguest::G(0x332ec0));
    if ((*(uint32_t*)rtguest::G(0x2a2bc4)) <= (*(float*)rtguest::G((uint32_t)fStack_54)) - 0.5) {
      PlayUiSound(0x31c);
      (*(uint8_t*)rtguest::G(0x33573a)) = 2;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x27))) = 1;
      switchD_002407e4_proximityEventNoOpCase();
      return 0;
    }
    break;
  case 2:
    (*(uint32_t*)rtguest::G(0x2a2bc0)) = (*(uint32_t*)rtguest::G(0x2a2bc0)) + (*(uint32_t*)rtguest::G(0x332ec8));
    (*(uint32_t*)rtguest::G(0x2a2bc4)) = ((*(float*)rtguest::G((uint32_t)fStack_54)) - 0.5) + (float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) * (*(uint32_t*)rtguest::G(0x332ec4));
    vu0SubVector(auStack_70,0x2a2bc0,0x177ace0);
    fVar6 = rtguest::fbits(vecDotProductVu0(auStack_70,auStack_70));
    if (SQRT(fVar6) < 1.5) {
      PlayUiSound(0x302);
      SetFlaggedSlotBit(0xf,0x2f,0);
      (*(uint8_t*)rtguest::G(0x33573a)) = 0;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    else {
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    fVar6 = (*(uint32_t*)rtguest::G(0x332ecc));
    if (bVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) + (*(uint32_t*)rtguest::G(0x332ecc));
      vu0CopyVector(auStack_60,0x2a2bc0);
      lVar3 = rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_60, auStack_50, auStack_4c, 3, param_2);
      if (lVar3 == 0x100651) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
      }
      else if (lVar3 == 0x101651) {
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
      }
      else {
        (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) - fVar6;
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) ^ 1;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = bVar1;
      }
    }
    fVar6 = (*(uint32_t*)rtguest::G(0x332ed0));
    if (bVar1 == 1) {
      (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) - (*(uint32_t*)rtguest::G(0x332ed0));
      vu0CopyVector(auStack_60,0x2a2bc0);
      lVar3 = rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_60, auStack_50, auStack_4c, 3, param_2);
      if ((lVar3 != 0x100651) && (lVar3 != 0x101651)) {
        (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) + fVar6;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26))) ^ 1;
      }
    }
    if ((*(uint32_t*)rtguest::G(0x2a2bc0)) < 1920.0) {
      return 0;
    }
    (*(uint8_t*)rtguest::G(0x33573a)) = 4;
    return 0;
  case 3:
    break;
  case 4:
    (*(uint32_t*)rtguest::G(0x2a2bc0)) = (*(uint32_t*)rtguest::G(0x2a2bc0)) + (*(uint32_t*)rtguest::G(0x332ed4));
    (*(uint32_t*)rtguest::G(0x2a2bc4)) = (*(uint32_t*)rtguest::G(0x2a2bc4)) - (*(uint32_t*)rtguest::G(0x332ed8));
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    if (cVar4 == '\0') {
      (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) + (*(uint32_t*)rtguest::G(0x332ed8));
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x26)));
    }
    if (cVar4 == '\x01') {
      (*(uint32_t*)rtguest::G(0x2a2bc8)) = (*(uint32_t*)rtguest::G(0x2a2bc8)) - (*(uint32_t*)rtguest::G(0x332edc));
    }
    if ((*(float*)rtguest::G((uint32_t)fStack_5c)) < (*(uint32_t*)rtguest::G(0x2a2bc4))) {
      return 0;
    }
    (*(uint8_t*)rtguest::G(0x33573a)) = 0;
    break;
  case 5:
    ::printf((char *)rtguest::G((uint32_t)(0x2ec910u)));
    gs_tasks_kill(param_1);
    break;
  default:
    goto switchD_002407e4_default;
  }
switchD_002407e4_default:
  return 0;
}


RT_GUEST_FN(0x002406f8, guest::UpdateProximityEvent)

}  // namespace guest
