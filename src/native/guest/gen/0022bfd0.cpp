// guest:0x0022bfd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunSceneIntroTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
  undefined8 uVar1;
  int iVar2;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  iVar2 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8))) == 0) {
    RT_CALLRA(0x22c018, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) | 0x60000));
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) == 0) {
      (*(uint32_t*)rtguest::G(0x177ea5c)) = 0x0177f7b4 + ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0xff) * 0xc;
      RT_CALLRA(0x22c064, guest::gs_tasks_spawn(param_1,0x20dc50,0x70301));
    }
    RT_CALLRA(0x22c094, guest::gifBeginSpriteBatch(0x17523f0,0x2004874e2131237a));
    RT_CALLRA(0x22c0a4, guest::gifSetSpriteCount(0x1752450,1));
    RT_CALLRA(0x22c0c8, guest::uiSpriteWriteZ(0x1752460,0x40,0x28,0,0,0x100,0xb8,0x100000));
    RT_CALLRA(0x22c0ec, guest::uiSetSpriteXY(0x1752480,0x17524a0,0x40,0x28,0x200,0x170));
    RT_CALLRA(0x22c0f4, guest::vu0UnitMatrix(0x17525d0));
    RT_CALLRA(0x22c104, guest::vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f36c)),0x17525d0,0x17525d0));
    RT_CALLRA(0x22c110, guest::vu0CopyVectorXYZ(0x1752600,0x29f360));
    (*(uint32_t*)rtguest::G(0x1752658)) = 0;
    (*(uint32_t*)rtguest::G(0x1752654)) = 0;
    (*(uint32_t*)rtguest::G(0x1752650)) = 0;
    RT_CALLRA(0x22c128, guest::vu0UnitMatrix(0x1752610));
    (*(uint32_t*)rtguest::G(0x1752644)) = (*(uint32_t*)rtguest::G(0x332e20));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f7bc + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) * 0xc))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x1752644)) = 0;
    }
  }
  else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
    RT_CALLRA(0x22c178, guest::UpdateSceneLightsAndFog(1,0));
    RT_CALLRA(0x22c184, guest::vu0UnitMatrix(auStack_70));
    RT_CALLRA(0x22c194, guest::SetViewDistanceGlobals(0x45000000,0x45000000));
    RT_CALLRA(0x22c1ac, guest::buildCamera(0x177e6d0,0x29f3b0,auStack_70,auStack_70));
    RT_CALLRA(0x22c1b4, guest::updateLightDirection(0x177e850));
    RT_CALLRA(0x22c1c4, guest::vifDirect(0x17523f0,0xc));
    RT_CALLRA(0x22c1cc, guest::vifUploadAt4(0x177e6d0));
    RT_CALLRA(0x22c1d4, guest::ResetShowroomLighting());
    uVar1 = 3;
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) != 0) {
      uVar1 = 0;
    }
    RT_CALLRA(0x22c1f0, guest::DrawCarVariant0(0x17525d0,uVar1));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0x50) != 0) {
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))) & 0x10000) == 0) {
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
        RT_CALLRA(0x22c258, guest::gs_tasks_kill(param_1));
      }
      else {
        RT_CALLRA(0x22c22c, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 2)) | 0xa0000));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x22c5f0;
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022bfd0, guest::RunSceneIntroTask)

}  // namespace guest
