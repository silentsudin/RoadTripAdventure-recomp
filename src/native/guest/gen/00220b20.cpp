// guest:0x00220b20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneCloudHill(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  undefined4 uVar1;
  long lVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x8000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) + -1;
  }
  updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180);
  vifBeginPacket(1);
  drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2);
  drawSkyLayers((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2);
  uVar6 = 0;
  vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))));
  vifUploadSceneParams(0x177f720);
  vifCameraSetup();
  gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d);
  vifDirect(0x172ff50,2);
  vifDirect(0x29cfc0,3);
  vifSetBatchOffset(0x29d0b0);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
  uVar5 = 0;
  do {
    lVar2 = cellVisible((float)((uVar6 & 7) * 200 + 100),(float)((uVar5 & 7) * 200 + 100),uVar1);
    iVar4 = uVar6 * 4;
    uVar6 = uVar6 + 1;
    if (lVar2 != 0) {
      vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + (*(int32_t*)rtguest::G(0x177ab28))))));
    }
    uVar5 = (int)uVar6 >> 3;
  } while ((int)uVar6 < 0x40);
  drawCarsField((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2);
  drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,1);
  vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))));
  vifUploadSceneParams(0x177f720);
  vifCameraSetup();
  DrawSceneArcRibbonFaded(param_2);
  vifSetBatchOffset(0x29d0b0);
  vifDirect(0x29cff0,3);
  vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100))));
  drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2);
  uVar3 = vifEndPacket(1);
  vifKick(uVar3);
  return 0;
}


RT_GUEST_FN(0x00220b20, guest::drawSceneCloudHill)

}  // namespace guest
