// guest:0x00220938 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneField(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  undefined8 uVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x8000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) + -1;
  }
  updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))) + 0x180);
  (*(uint8_t*)rtguest::G(0x3335b8)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335bb)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335ba)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335b9)) = 0xff;
  iVar1 = GetGridCellIndex(0x0177acf0 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c);
  (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(iVar1) * 1)) = 3;
  DrawFieldTerrainTiles
            ((*(int32_t*)rtguest::G((uint32_t)0x177acf0 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c) * 4)),
             (*(uint32_t*)rtguest::G((uint32_t)0x177acf8 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c) * 4)),param_1,param_2);
  vifBeginPacket(1);
  drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2);
  drawSkyLayers((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2);
  vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))));
  vifUploadSceneParams(0x177f720);
  vifCameraSetup();
  gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d);
  vifDirect(0x172ff50,2);
  vifDirect(0x29cfc0,3);
  drawFieldTiles((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2);
  drawCarsField((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2);
  drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2,1);
  drawCoins(param_1,param_2);
  drawAnimatedProps(param_1,param_2);
  vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))));
  vifUploadSceneParams(0x177f720);
  vifCameraSetup();
  DrawSceneArcRibbonFaded(param_2);
  vifDirect(0x29cff0,3);
  drawFieldTilesExtra(param_2);
  drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))),param_2);
  uVar2 = vifEndPacket(1);
  vifKick(uVar2);
  return 0;
}


RT_GUEST_FN(0x00220938, guest::drawSceneField)

}  // namespace guest
