// guest:0x00220d40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneRace(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
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
    RT_CALLRA(0x220d9c, guest::updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar3 != 2) {
      RT_CALLRA(0x220db4, guest::vifBeginPacket(1));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar3 != 0) {
      RT_CALLRA(0x220dd4, guest::vifDirect(0x0029d020 + iVar3 * 0x20,2));
    }
    RT_CALLRA(0x220de0, guest::drawSkyDome((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2));
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22))) == '\x11') {
      RT_CALLRA(0x220df8, guest::drawSkyLayers(uVar4,param_2));
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    RT_CALLRA(0x220e0c, guest::vifUploadCamera(uVar4));
    RT_CALLRA(0x220e14, guest::vifUploadSceneParams(0x177f720));
    RT_CALLRA(0x220e1c, guest::vifCameraSetup());
    RT_CALLRA(0x220e2c, guest::gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d));
    RT_CALLRA(0x220e3c, guest::vifDirect(0x172ff50,2));
    RT_CALLRA(0x220e44, guest::vifSetBatchOffset(0x29d0b0));
    RT_CALLRA(0x220e58, guest::vifDirect(0x29cfc0,3));
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    uVar6 = 0;
    uVar5 = 0;
    do {
      lVar1 = RT_CALLRA(0x220ec0, guest::cellVisible((float)((uVar6 & 7) * 200 + 100),(float)((uVar5 & 7) * 200 + 100),uVar4));
      iVar3 = uVar6 * 4;
      uVar6 = uVar6 + 1;
      if (lVar1 != 0) {
        RT_CALLRA(0x220edc, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab28)))))));
      }
      uVar5 = (int)uVar6 >> 3;
    } while ((int)uVar6 < 0x40);
    uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x22))) - 0x19u < 2) {
      RT_CALLRA(0x220f04, guest::drawRaceProps(uVar4,param_2));
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    RT_CALLRA(0x220f10, guest::drawCarsField(uVar4,param_2));
    RT_CALLRA(0x220f20, guest::drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,0));
    RT_CALLRA(0x220f28, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)))));
    RT_CALLRA(0x220f30, guest::vifUploadSceneParams(0x177f720));
    RT_CALLRA(0x220f38, guest::vifCameraSetup());
    RT_CALLRA(0x220f40, guest::vifSetBatchOffset(0x29d0b0));
    RT_CALLRA(0x220f50, guest::vifDirect(0x29cff0,3));
    RT_CALLRA(0x220f60, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100)))));
    RT_CALLRA(0x220f70, guest::drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2));
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar7 == 2) {
      RT_CALLRA(0x220f90, guest::vifDirect(0x29d020,2));
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar7 != 1) {
      uVar2 = RT_CALLRA(0x220fa8, guest::vifEndPacket(1));
      RT_CALLRA(0x220fb0, guest::vifKick(uVar2));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00220d40, guest::drawSceneRace)

}  // namespace guest
