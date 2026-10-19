// guest:0x00240cd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTrafficHazard(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
  int iVar3;
  long lVar4;
  int iVar5;
uint32_t puVar6;
  int iVar7;
  int iVar8;
  float fVar9;
  
  lVar4 = TestStateBitFc78(0x11);
  if ((lVar4 != 0) || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8))) == 0)) {
    (*(uint32_t*)rtguest::G(0x2a2bd8)) = (*(uint32_t*)rtguest::G(0x332ee0));
    (*(uint32_t*)rtguest::G(0x177af68)) = 0x74428f4;
    (*(uint32_t*)rtguest::G(0x177af60)) = 0x7ae3d70;
    (*(uint32_t*)rtguest::G(0x177af64)) = 0x000f5bf5;
    (*(int8_t*)rtguest::G(0x335739)) = 0;
  }
  iVar5 = (int)param_2;
  if ((*(int8_t*)rtguest::G(0x335739)) == 1) {
    if ((*(uint32_t*)rtguest::G(0x2a2bd8)) <= (*(uint32_t*)rtguest::G(0x332ee4))) {
      (*(int8_t*)rtguest::G(0x335739)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
    }
    else {
      (*(uint32_t*)rtguest::G(0x2a2bd8)) = (*(uint32_t*)rtguest::G(0x2a2bd8)) - (*(uint32_t*)rtguest::G(0x332ee8));
    }
  }
  else if ((*(int8_t*)rtguest::G(0x335739)) < 2) {
    if (((*(int8_t*)rtguest::G(0x335739)) == 0) && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54))) == 0x17)) {
      lVar4 = TestFlagBitFc98(0x50);
      if (((lVar4 == 0) &&
          (((lVar4 = TestStateBitFc78(0x11), lVar4 == 0 &&
            (0x2327f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) - 54000U)) &&
           (lVar4 = IsPointInsideQuad(0x2a3880,0x177ac50), lVar4 != 0)))) && ((*(uint32_t*)rtguest::G(0x177ace4)) < 50.0))
      {
        (*(int8_t*)rtguest::G(0x335739)) = 1;
        (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
        ResetVehicleState(param_2,0x177ac50);
      }
      puVar6 = 0x177ac50;
      fVar9 = 50.0;
      iVar7 = 0xb;
      do {
        lVar4 = IsPointInsideQuad(0x2a38a0,puVar6);
        if ((lVar4 != 0) && ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0x94))) < fVar9)) {
          iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1e)));
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x3a))) = 0;
          iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xf4)));
          iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1f)));
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1e))) = (int)((float)iVar8 * 0.75);
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x14))) =
               (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x14))) + ((iVar8 << 5) / 0x19) * -2 & 0xfffffff;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xf4))) = (int)((float)iVar1 * 0.75);
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xa4))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xa4))) + ((iVar1 << 5) / 0x19) * -2;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1f))) = (int)((float)iVar2 * 0.75);
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x15))) =
               (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x15))) + ((iVar2 << 5) / 0x19) * -2 & 0xfffffff;
        }
        iVar7 = iVar7 + -1;
        puVar6 = puVar6 + 0x4e;
      } while (-1 < iVar7);
      lVar4 = TestStateBitFc78(0x11);
      if (((lVar4 != 0) || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) - 54000U < 0x23280)) &&
         (lVar4 = IsPointInsideQuad(0x2a38e0,0x177ac50), lVar4 != 0)) {
        (*(uint32_t*)rtguest::G(0x177ae20)) = 0;
        iVar7 = ((*(uint32_t*)rtguest::G(0x177ad48)) << 5) / 0x19;
        if (iVar7 < 0) {
          iVar7 = -iVar7;
        }
        (*(uint32_t*)rtguest::G(0x177acf8)) = (*(uint32_t*)rtguest::G(0x177acf8)) + iVar7 * 2 & 0xfffffff;
      }
      lVar4 = IsPointInsideQuad(0x2a3900,0x177ac50);
      if ((lVar4 != 0) && ((*(uint32_t*)rtguest::G(0x177ace4)) < 50.0)) {
        (*(uint32_t*)rtguest::G(0x177acf8)) = 0x74429c9;
        (*(int32_t*)rtguest::G(0x177acf0)) = 0x7af65a1;
        (*(uint32_t*)rtguest::G(0x177acf4)) = 0x000f5bf5;
      }
    }
  }
  else if ((*(int8_t*)rtguest::G(0x335739)) == 2) {
    iVar8 = 0;
    iVar7 = 0;
    do {
      if ((((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54))) == 0x17) &&
          (lVar4 = IsPointInsideQuad(0x2a38c0,(int)0x177ac50 + iVar7), lVar4 != 0)) &&
         ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ace4 + iVar7))) < 50.0)) {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ad40 + iVar7)));
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ae20 + iVar7))) = 0;
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ad44 + iVar7)));
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ad48 + iVar7)));
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ad40 + iVar7))) = (int)((float)iVar1 * 0.75);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177ad48 + iVar7))) = (int)((float)iVar3 * 0.75);
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf0 + iVar7))) =
             (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf0 + iVar7))) + ((iVar1 << 5) / 0x19) * -2 & 0xfffffff;
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ad44 + iVar7))) = (int)((float)iVar2 * 0.75);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf4 + iVar7))) =
             (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf4 + iVar7))) + ((iVar2 << 5) / 0x19) * -2;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf8 + iVar7))) =
             (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177acf8 + iVar7))) + ((iVar3 << 5) / 0x19) * -2 & 0xfffffff;
      }
      iVar8 = iVar8 + 1;
      iVar7 = iVar7 + 0x270;
    } while (iVar8 < 0xc);
  }
  else if ((*(int8_t*)rtguest::G(0x335739)) == 3) {
    ::printf((char *)rtguest::G((uint32_t)(0x2ec938u)));
    gs_tasks_kill(param_1);
  }
  lVar4 = TestStateBitFc78(0x11);
  if (((lVar4 == 0) && (53999 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))))) && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) < 0x30570)) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54)));
  }
  else {
    lVar4 = TestFlagBitFc98(0x50);
    if (lVar4 == 0) {
      return 0;
    }
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54)));
  }
  if (iVar5 == 0x17) {
    (*(uint32_t*)rtguest::G(0x177af68)) = 0x74428f4;
    (*(uint32_t*)rtguest::G(0x177af60)) = 0x7ae3d70;
    (*(uint32_t*)rtguest::G(0x177af64)) = 0x000f5bf5;
  }
}


RT_GUEST_FN(0x00240cd8, guest::UpdateTrafficHazard)

}  // namespace guest
