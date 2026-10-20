// guest:0x00216e00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateWorldMapMenuTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
uint32_t auStack_50 = rtguest::allocFrame(16);
    uint32_t uStack_40 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
  ushort uVar1;
  int iVar2;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 8))) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
    (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0') {
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
  if (iVar2 == 100) {
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
LAB_00216e88:
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
LAB_00216e8c:
    if ((uVar1 & 0x10) == 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
      goto LAB_00216eb8;
    }
    PlayUiSound(0x1b);
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 399;
  }
  else {
    if (iVar2 < 0x65) {
      if (iVar2 == 0) {
        (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0x65;
        goto LAB_00216e88;
      }
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    }
    else {
      if (iVar2 == 0x65) {
        uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30)));
        goto LAB_00216e8c;
      }
      (*(int32_t*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(int32_t))) = 0;
    }
    (*(int32_t*)rtguest::G((uint32_t)param_4)) = 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_4));
LAB_00216eb8:
  if ((iVar2 == 299) || (iVar2 == 399)) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 4))) = 0;
  }
  param_3 = param_3 + 0x20;
  uiArenaInit(param_3,0x8000);
  uiArenaBeginBatch(param_3,0,0);
  uiArenaSetTexture(param_3,0x29c030,8,0);
  uiArenaDataTag(param_3,1);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_50)), 0, 0x20));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_3c)) = 0x1c0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_40)) = 0x280;
  uiArenaSprites(param_3,auStack_50,1,0,0,2);
  if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))) != '\"')) {
    DrawTextHalfY(((*(int32_t*)rtguest::G(0x177acf0)) + 0x2000000U & 0xfffffff) / 0x91140 + 0x82,
                  0x1a2 - (*(uint32_t*)rtguest::G(0x177acf8)) / 0x91140,0x333580,4);
  }
  uiArenaSubmit(param_3);
  return 0;
}


RT_GUEST_FN(0x00216e00, guest::UpdateWorldMapMenuTask)

}  // namespace guest
