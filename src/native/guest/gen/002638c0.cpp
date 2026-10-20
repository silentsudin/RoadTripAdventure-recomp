// guest:0x002638c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceToPointTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
uint32_t auStack_50 = rtguest::allocFrame(16);
  short sVar1;
  int iVar2;
uint32_t __s;
  int iVar3;
  int iVar4;
uint32_t puVar5;
  float fVar6;
  
  iVar4 = (int)param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0x3f80000044236666;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x42ce6b8544567333;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x8cc0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    InitObjectType25((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))));
    gs_music_play_bgm(0xa100201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))) + 0x8cb0))) = 0;
    (*(uint8_t*)rtguest::G(0x177b05a)) = 1;
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28)));
  puVar5 = (uint32_t)(iVar2 + 0x8cb0);
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar5))) {
  case 0:
    StoreIndexedPosition(0x43b10000,0x42cf0000,(*(uint32_t*)rtguest::G(0x3331bc)),0,0xffffffffffffc000);
    StoreIndexedPosition(0x43b10000,(*(uint32_t*)rtguest::G(0x3331c0)),(*(uint32_t*)rtguest::G(0x3331c4)),1,0xffffffffffffc000);
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 1;
  case 1:
    vu0SubVector(auStack_50,uStack_60,0x177ace0);
    fVar6 = rtguest::fbits(vecDotProductVu0(auStack_50,auStack_50));
    if (SQRT(fVar6) < 1.5) {
      GetOrSetPresidentRaceResult(2,uStack_60);
      SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))),0,1);
      PlayUiSound(0x302);
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 1;
    }
    vu0SubVector(auStack_50,uStack_60,0x177af50);
    fVar6 = rtguest::fbits(vecDotProductVu0(auStack_50,auStack_50));
    if (1.5 <= SQRT(fVar6)) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc)));
      goto LAB_00263ba4;
    }
    GetOrSetPresidentRaceResult(1);
    SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))),0,1);
    PlayUiSound(0x302);
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 1;
    break;
  case 2:
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 3;
    break;
  case 3:
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2)))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2))) + -0x40) * 0x1000000);
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2)));
    }
    if (sVar1 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 2))) = sVar1 + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0xffffffff;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 1;
    break;
  default:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc)));
    goto LAB_00263ba4;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc)));
LAB_00263ba4:
  if (iVar3 != 0) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) & 0xffffffff;
  }
  vu0UnitMatrix(0x177eb10);
  vu0CopyVector(0x177eb40,uStack_60);
  raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x28))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x002638c0, guest::raceToPointTask)

}  // namespace guest
