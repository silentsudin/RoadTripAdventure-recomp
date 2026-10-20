// guest:0x00220b20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneCloudHill(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  undefined4 uVar1;
  long lVar2;
  undefined8 uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x8000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) + -1;
  }
  RT_CALLRA(0x220b80, guest::updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180));
  RT_CALLRA(0x220b88, guest::vifBeginPacket(1));
  RT_CALLRA(0x220b98, guest::drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2));
  RT_CALLRA(0x220ba8, guest::drawSkyLayers((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2));
  uVar6 = 0;
  RT_CALLRA(0x220bb8, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)))));
  RT_CALLRA(0x220bc0, guest::vifUploadSceneParams(0x177f720));
  RT_CALLRA(0x220bc8, guest::vifCameraSetup());
  RT_CALLRA(0x220bd8, guest::gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d));
  RT_CALLRA(0x220be4, guest::vifDirect(0x172ff50,2));
  RT_CALLRA(0x220bf4, guest::vifDirect(0x29cfc0,3));
  RT_CALLRA(0x220bfc, guest::vifSetBatchOffset(0x29d0b0));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
  uVar5 = 0;
  do {
    lVar2 = RT_CALLRA(0x220c60, guest::cellVisible((float)((uVar6 & 7) * 200 + 100),(float)((uVar5 & 7) * 200 + 100),uVar1));
    iVar4 = uVar6 * 4;
    uVar6 = uVar6 + 1;
    if (lVar2 != 0) {
      RT_CALLRA(0x220c7c, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + (*(int32_t*)rtguest::G(0x177ab28)))))));
    }
    uVar5 = (int)uVar6 >> 3;
  } while ((int)uVar6 < 0x40);
  RT_CALLRA(0x220c94, guest::drawCarsField((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2));
  RT_CALLRA(0x220ca4, guest::drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,1));
  RT_CALLRA(0x220cac, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)))));
  RT_CALLRA(0x220cb4, guest::vifUploadSceneParams(0x177f720));
  RT_CALLRA(0x220cbc, guest::vifCameraSetup());
  RT_CALLRA(0x220cc4, guest::DrawSceneArcRibbonFaded(param_2));
  RT_CALLRA(0x220ccc, guest::vifSetBatchOffset(0x29d0b0));
  RT_CALLRA(0x220cdc, guest::vifDirect(0x29cff0,3));
  RT_CALLRA(0x220cec, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100)))));
  RT_CALLRA(0x220cfc, guest::drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2));
  uVar3 = RT_CALLRA(0x220d04, guest::vifEndPacket(1));
  vifKick(uVar3);
  return 0;
}


RT_GUEST_FN(0x00220b20, guest::drawSceneCloudHill)

}  // namespace guest
