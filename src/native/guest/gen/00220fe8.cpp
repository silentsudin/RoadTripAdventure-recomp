// guest:0x00220fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneAction(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  undefined8 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  uint uVar7;
  
  iVar3 = (int)param_2;
  uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
  if ((uVar6 & 0xa000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) + -1;
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)));
  }
  if ((uVar6 & 0x2000) == 0) {
    updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180);
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar4 != 2) {
      vifBeginPacket(1);
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar4 != 0) {
      vifDirect(0x0029d020 + iVar4 * 0x20,2);
    }
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x22))) == ' ') {
      drawSkyDome(uVar5,param_2);
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    vifUploadCamera(uVar5);
    vifUploadSceneParams(0x177f720);
    vifCameraSetup();
    gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d);
    vifDirect(0x172ff50,2);
    vifDirect(0x29cfc0,3);
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    uVar7 = 0;
    uVar6 = 0;
    do {
      lVar1 = cellVisible((float)((uVar7 & 7) * 200 + 100),(float)((uVar6 & 7) * 200 + 100),uVar5);
      iVar3 = uVar7 * 4;
      uVar7 = uVar7 + 1;
      if (lVar1 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab28))))));
      }
      uVar6 = (int)uVar7 >> 3;
    } while ((int)uVar7 < 0x40);
    drawCarsAction((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2);
    drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,0);
    vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))));
    vifUploadSceneParams(0x177f720);
    vifCameraSetup();
    vifSetBatchOffset(0x29d0b0);
    vifDirect(0x29cff0,3);
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100))));
    drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar3 == 2) {
      vifDirect(0x29d020,2);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar3 != 1) {
      uVar2 = vifEndPacket(1);
      vifKick(uVar2);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00220fe8, guest::drawSceneAction)

}  // namespace guest
