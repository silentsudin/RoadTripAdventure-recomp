// guest:0x00220fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawSceneAction(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
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
    RT_CALLRA(0x221040, guest::updateLightDirection((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) + 0x180));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar4 != 2) {
      RT_CALLRA(0x221058, guest::vifBeginPacket(1));
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar4 != 0) {
      RT_CALLRA(0x221078, guest::vifDirect(0x0029d020 + iVar4 * 0x20,2));
    }
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x22))) == ' ') {
      RT_CALLRA(0x221090, guest::drawSkyDome(uVar5,param_2));
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    }
    RT_CALLRA(0x2210a4, guest::vifUploadCamera(uVar5));
    RT_CALLRA(0x2210ac, guest::vifUploadSceneParams(0x177f720));
    RT_CALLRA(0x2210b4, guest::vifCameraSetup());
    RT_CALLRA(0x2210c4, guest::gifAdPair(0x172ff50,(*(uint32_t*)rtguest::G(0x177f750)),0x3d));
    RT_CALLRA(0x2210d4, guest::vifDirect(0x172ff50,2));
    RT_CALLRA(0x2210e4, guest::vifDirect(0x29cfc0,3));
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    uVar7 = 0;
    uVar6 = 0;
    do {
      lVar1 = RT_CALLRA(0x221150, guest::cellVisible((float)((uVar7 & 7) * 200 + 100),(float)((uVar6 & 7) * 200 + 100),uVar5));
      iVar3 = uVar7 * 4;
      uVar7 = uVar7 + 1;
      if (lVar1 != 0) {
        RT_CALLRA(0x22116c, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + (*(int32_t*)rtguest::G(0x177ab28)))))));
      }
      uVar6 = (int)uVar7 >> 3;
    } while ((int)uVar7 < 0x40);
    RT_CALLRA(0x221184, guest::drawCarsAction((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2));
    RT_CALLRA(0x221194, guest::drawFieldProps((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),param_2,0));
    RT_CALLRA(0x22119c, guest::vifUploadCamera((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)))));
    RT_CALLRA(0x2211a4, guest::vifUploadSceneParams(0x177f720));
    RT_CALLRA(0x2211ac, guest::vifCameraSetup());
    RT_CALLRA(0x2211b8, guest::vifSetBatchOffset(0x29d0b0));
    RT_CALLRA(0x2211c8, guest::vifDirect(0x29cff0,3));
    RT_CALLRA(0x2211d8, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100)))));
    RT_CALLRA(0x2211e8, guest::drawCarEffects((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))),param_2));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    if (iVar3 == 2) {
      RT_CALLRA(0x221208, guest::vifDirect(0x29d020,2));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
    }
    if (iVar3 != 1) {
      uVar2 = RT_CALLRA(0x221220, guest::vifEndPacket(1));
      RT_CALLRA(0x221228, guest::vifKick(uVar2));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00220fe8, guest::drawSceneAction)

}  // namespace guest
