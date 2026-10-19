// guest:0x00220d40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneRace(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  undefined8 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  
  iVar7 = (int)param_2;
  uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  if ((uVar5 & 0xa000) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) + -1;
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x28)));
  }
  if ((uVar5 & 0x2000) == 0) {
    updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180);
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar3 != 2) {
      vifBeginPacket(1);
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar3 != 0) {
      vifDirect(0x0029d020 + iVar3 * 0x20,2);
    }
    drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22))) == '\x11') {
      drawSkyLayers(uVar4,param_2);
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    vifUploadCamera(uVar4);
    vifUploadSceneParams(0x177f720);
    vifCameraSetup();
    gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d);
    vifDirect(0x172ff50,2);
    vifSetBatchOffset(0x29d0b0);
    vifDirect(0x29cfc0,3);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    uVar6 = 0;
    uVar5 = 0;
    do {
      lVar1 = cellVisible((float)((uVar6 & 7) * 200 + 100),(float)((uVar5 & 7) * 200 + 100),uVar4);
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar1 != 0) {
        vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab28))))));
      }
      uVar5 = (int)uVar6 >> 3;
    } while ((int)uVar6 < 0x40);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22))) - 0x19 < 2) {
      drawRaceProps(uVar4,param_2);
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    drawCarsField(uVar4,param_2);
    drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,0);
    vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))));
    vifUploadSceneParams(0x177f720);
    vifCameraSetup();
    vifSetBatchOffset(0x29d0b0);
    vifDirect(0x29cff0,3);
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100))));
    drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2);
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar7 == 2) {
      vifDirect(0x29d020,2);
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar7 != 1) {
      uVar2 = vifEndPacket(1);
      vifKick(uVar2);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00220d40, guest::drawSceneRace)

}  // namespace guest
