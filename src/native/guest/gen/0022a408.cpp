// guest:0x0022a408 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyMotionClipFrame(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t fStack_68 = rtguest::allocFrame(4);
    uint32_t uStack_64 = rtguest::allocFrame(4);
uint32_t auStack_60 = rtguest::allocFrame(16);
  char cVar1;
  byte bVar2;
  short sVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
uint32_t psVar7;
uint32_t psVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  int iParam4;
  
  fVar10 = (*(float*)rtguest::G(0x332dd4));
  iVar6 = (int)param_1;
  psVar8 = (uint32_t)(iVar6 + 0x8c);
  psVar7 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(0x29f300u + (uint32_t)(param_3) * 4)));
  iParam4 = (int)param_4;
  sVar3 = (*(int16_t*)rtguest::G((uint32_t)psVar7));
  if ((*(int16_t*)rtguest::G((uint32_t)psVar7)) != 0) {
    fVar11 = 12800.0;
    do {
      bVar4 = iParam4 < sVar3;
      iParam4 = iParam4 - sVar3;
      if (bVar4) {
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + sizeof(int16_t))));
        if ('\0' < (char)(*(int16_t*)rtguest::G((uint32_t)psVar7 + (uint32_t)(1) * sizeof(int16_t)))) {
          (*(int16_t*)rtguest::G((uint32_t)psVar8)) = (*(int16_t*)rtguest::G((uint32_t)psVar8)) + 0x23;
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + sizeof(int16_t))));
        }
        if ((int)((uint32_t)bVar2 << 0x18) < 0) {
          (*(int16_t*)rtguest::G((uint32_t)psVar8)) = (*(int16_t*)rtguest::G((uint32_t)psVar8)) + -0x23;
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 3)));
        }
        else {
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 3)));
        }
        if (cVar1 == '\0') {
          fVar9 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84)));
          if ((*(float*)rtguest::G(0x332de0)) <= fVar9) {
            if ((*(float*)rtguest::G(0x332de4)) < fVar9) {
              (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84))) = fVar9 - (*(float*)rtguest::G(0x332de4));
            }
            else {
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84))) = 0;
            }
            goto LAB_0022a570;
          }
        }
        else {
          iVar5 = (int)(*(int16_t*)rtguest::G((uint32_t)psVar8)) * (int)cVar1;
          if (iVar5 < 0) {
            iVar5 = iVar5 + 3;
          }
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x8e))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x8e))) + (short)(iVar5 >> 2);
          fVar9 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84)));
          if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar7 + 3))) < '\x01') {
            if ((*(float*)rtguest::G(0x332ddc)) < fVar9) {
              (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84))) = fVar9 - fVar10;
            }
            goto LAB_0022a570;
          }
          if ((*(float*)rtguest::G(0x332dd8)) <= fVar9) goto LAB_0022a570;
        }
        (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x84))) = fVar9 + fVar10;
LAB_0022a570:
        vu0CopyVector(auStack_60,iVar6 + 0x30);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_64)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
        (*(float*)rtguest::G((uint32_t)fStack_68)) = (float)(int)(*(int16_t*)rtguest::G((uint32_t)psVar8)) / fVar11;
        vu0UnitMatrix(param_1);
        vu0RotMatrixY((float)(int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x8e))) * 3.0517578e-05 * (*(float*)rtguest::G(0x332de8)),param_1,
                      param_1);
        vu0ApplyMatrix(uStack_70,param_1,uStack_70);
        vu0AddVector(iVar6 + 0x30,auStack_60,uStack_70);
        fVar10 = ((float)(int)(*(int16_t*)rtguest::G((uint32_t)psVar8)) / fVar11) / (*(float*)rtguest::G(0x332dec));
        (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x88))) =
             (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x88))) + ((float)(int)(*(int16_t*)rtguest::G((uint32_t)psVar8)) / fVar11) / (*(float*)rtguest::G(0x332df0));
        (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x80))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x80))) + fVar10;
        WrapCoordinate(iVar6 + 0x80);
        WrapCoordinate(iVar6 + 0x88);
        (*(uint8_t*)rtguest::G((uint32_t)param_2)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(psVar7 + sizeof(int16_t)))) & 2;
        return 1;
      }
      psVar7 = psVar7 + 2 * sizeof(int16_t);
      sVar3 = (*(int16_t*)rtguest::G((uint32_t)psVar7));
    } while ((*(int16_t*)rtguest::G((uint32_t)psVar7)) != 0);
  }
  return 0;
}


RT_GUEST_FN(0x0022a408, guest::ApplyMotionClipFrame)

}  // namespace guest
