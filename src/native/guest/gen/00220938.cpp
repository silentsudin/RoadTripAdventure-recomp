// guest:0x00220938 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneField(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  undefined8 uVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x8000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) + -1;
  }
  RT_CALLRA(0x220980, guest::updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))) + 0x180));
  (*(uint8_t*)rtguest::G(0x3335b8)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335bb)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335ba)) = 0xff;
  (*(uint32_t*)rtguest::G(0x3335b9)) = 0xff;
  iVar1 = RT_CALLRA(0x2209b4, guest::GetGridCellIndex(0x0177acf0 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c));
  (*(uint8_t*)rtguest::G((uint32_t)0x3335b8 + (uint32_t)(iVar1) * 1)) = 3;
  RT_CALLRA(0x220a04, guest::DrawFieldTerrainTiles((*(int32_t*)rtguest::G((uint32_t)0x177acf0 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c) * 4)),
             (*(uint32_t*)rtguest::G((uint32_t)0x177acf8 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x20))) * 0x9c) * 4)),param_1,param_2));
  RT_CALLRA(0x220a0c, guest::vifBeginPacket(1));
  RT_CALLRA(0x220a18, guest::drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
  RT_CALLRA(0x220a24, guest::drawSkyLayers((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
  RT_CALLRA(0x220a2c, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)))));
  RT_CALLRA(0x220a34, guest::vifUploadSceneParams(0x177f720));
  RT_CALLRA(0x220a3c, guest::vifCameraSetup());
  RT_CALLRA(0x220a4c, guest::gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d));
  RT_CALLRA(0x220a58, guest::vifDirect(0x172ff50,2));
  RT_CALLRA(0x220a68, guest::vifDirect(0x29cfc0,3));
  RT_CALLRA(0x220a74, guest::drawFieldTiles((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
  RT_CALLRA(0x220a80, guest::drawCarsField((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2));
  RT_CALLRA(0x220a90, guest::drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),param_2,1));
  RT_CALLRA(0x220a9c, guest::drawCoins(param_1,param_2));
  RT_CALLRA(0x220aa8, guest::drawAnimatedProps(param_1,param_2));
  RT_CALLRA(0x220ab0, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28)))));
  RT_CALLRA(0x220ab8, guest::vifUploadSceneParams(0x177f720));
  RT_CALLRA(0x220ac0, guest::vifCameraSetup());
  RT_CALLRA(0x220ac8, guest::DrawSceneArcRibbonFaded(param_2));
  RT_CALLRA(0x220ad8, guest::vifDirect(0x29cff0,3));
  RT_CALLRA(0x220ae0, guest::drawFieldTilesExtra(param_2));
  RT_CALLRA(0x220af0, guest::drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))),param_2));
  uVar2 = RT_CALLRA(0x220af8, guest::vifEndPacket(1));
  vifKick(uVar2);
  return 0;
}


RT_GUEST_FN(0x00220938, guest::drawSceneField)

}  // namespace guest
