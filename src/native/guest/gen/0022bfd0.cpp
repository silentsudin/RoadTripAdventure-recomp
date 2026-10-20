// guest:0x0022bfd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunSceneIntroTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  undefined8 uVar1;
  int iVar2;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) | 0x60000);
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) == 0) {
      (*(uint32_t*)rtguest::G(0x177ea5c)) = 0x0177f7b4 + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0xff) * 0xc;
      gs_tasks_spawn(param_1,0x20dc50,0x70301);
    }
    gifBeginSpriteBatch(0x17523f0,0x2004874e2131237a);
    gifSetSpriteCount(0x1752450,1);
    uiSpriteWriteZ(0x1752460,0x40,0x28,0,0,0x100,0xb8,0x100000);
    uiSetSpriteXY(0x1752480,0x17524a0,0x40,0x28,0x200,0x170);
    vu0UnitMatrix(0x17525d0);
    vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f36c)),0x17525d0,0x17525d0);
    vu0CopyVectorXYZ(0x1752600,0x29f360);
    (*(uint32_t*)rtguest::G(0x1752658)) = 0;
    (*(uint32_t*)rtguest::G(0x1752654)) = 0;
    (*(uint32_t*)rtguest::G(0x1752650)) = 0;
    vu0UnitMatrix(0x1752610);
    (*(uint32_t*)rtguest::G(0x1752644)) = (*(uint32_t*)rtguest::G(0x332e20));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f7bc + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) * 0xc))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x1752644)) = 0;
    }
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
    UpdateSceneLightsAndFog(1,0);
    vu0UnitMatrix(auStack_70);
    SetViewDistanceGlobals(0x45000000,0x45000000);
    buildCamera(0x177e6d0,0x29f3b0,auStack_70,auStack_70);
    updateLightDirection(0x177e850);
    vifDirect(0x17523f0,0xc);
    vifUploadAt4(0x177e6d0);
    ResetShowroomLighting();
    uVar1 = 3;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) != 0) {
      uVar1 = 0;
    }
    DrawCarVariant0(0x17525d0,uVar1);
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0) {
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) == 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
        gs_tasks_kill(param_1);
      }
      else {
        gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2)) | 0xa0000);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x22c5f0;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022bfd0, guest::RunSceneIntroTask)

}  // namespace guest
