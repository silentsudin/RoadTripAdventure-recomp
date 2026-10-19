// guest:0x00259138 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnDoorMenuEntity(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t fStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_70 = rtguest::allocFrame(4);
uint32_t auStack_6c = rtguest::allocFrame(12);
  int iVar1;
  long lVar2;
  int iVar3;
uint32_t pfVar4;
  int iVar5;
  uint uVar6;
  long lVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  
  iVar5 = (int)param_2;
  if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) & 0x400) == 0) && (uVar6 = (*(uint16_t*)rtguest::G(0x1780a3a)) & 1, uVar6 < 0x16)) {
    do {
      iVar3 = uVar6 * 8;
      if ((long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + iVar3))) == (long)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54)))) {
        lVar7 = 0;
        pfVar4 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2c0698 + (uint32_t)(uVar6) * 4));
        if ('\0' < (char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1))) {
          do {
            if ((uVar6 == 9) && (lVar2 = IsMapEntryEnabled(lVar7), lVar2 == 0)) {
              lVar2 = (long)(*(uint32_t*)rtguest::G(0x2be486));
            }
            else if (uVar6 == 0xc) {
              if (((lVar7 == 0) && (0xe483 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))))) &&
                 ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) < 0x278d0)) {
                lVar2 = (long)(*(uint8_t*)rtguest::G(0x002be49e));
              }
              else {
                if (lVar7 != 0) {
                  fVar12 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float)));
                  goto LAB_00259274;
                }
                lVar2 = TestFlagBitFc98(0x50);
                if (lVar2 == 0) goto LAB_00259270;
                lVar2 = (long)(*(uint8_t*)rtguest::G(0x002be49e));
              }
            }
            else {
LAB_00259270:
              fVar12 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(1) * sizeof(float)));
LAB_00259274:
              fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(2) * sizeof(float)));
              fVar9 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(3) * sizeof(float)));
              fVar13 = (*(float*)rtguest::G((uint32_t)pfVar4));
              if (0.0 < (fVar9 - fVar12) * ((*(uint32_t*)rtguest::G(0x177ace0)) - fVar11) -
                        (fVar11 - fVar13) * ((*(uint32_t*)rtguest::G(0x177ace8)) - fVar9)) {
                fVar10 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(4) * sizeof(float)));
                fVar8 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(5) * sizeof(float)));
                if (0.0 < (fVar8 - fVar9) * ((*(uint32_t*)rtguest::G(0x177ace0)) - fVar10) -
                          (fVar10 - fVar11) * ((*(uint32_t*)rtguest::G(0x177ace8)) - fVar8)) {
                  fVar11 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(6) * sizeof(float)));
                  fVar9 = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(7) * sizeof(float)));
                  if (0.0 < (fVar9 - fVar8) * ((*(uint32_t*)rtguest::G(0x177ace0)) - fVar11) -
                            (fVar11 - fVar10) * ((*(uint32_t*)rtguest::G(0x177ace8)) - fVar9)) {
                    if (0.0 < (fVar12 - fVar9) * ((*(uint32_t*)rtguest::G(0x177ace0)) - fVar13) -
                              (fVar13 - fVar11) * ((*(uint32_t*)rtguest::G(0x177ace8)) - fVar12)) {
                      (*(float*)rtguest::G((uint32_t)fStack_80)) = (fVar10 + fVar11) * 0.5;
                      (*(float*)rtguest::G((uint32_t)fStack_7c)) = -20.0;
                      (*(float*)rtguest::G((uint32_t)fStack_78)) = ((*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(5) * sizeof(float))) + (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(7) * sizeof(float)))) * 0.5;
                      rtCall((*(uint32_t*)rtguest::G(0x335710)), fStack_80, auStack_70, auStack_6c, (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54))) & 3, param_2);
                      if ((*(uint32_t*)rtguest::G(0x177ace4)) < (*(float*)rtguest::G((uint32_t)fStack_7c)) + 1.0) {
                        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23))) = (char)uVar6;
                        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x21))) = (char)lVar7;
                        if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) & 0x1000) == 0) ||
                           (lVar2 = IsSlotActive(param_2), lVar2 != 0)) {
                          iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
                          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
                          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
                          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) | 0x400000;
                        }
                        else {
                          uiDrawText(0x82,0x71,0x328780,6);
                          uiDrawText(0x82,0x7d,0x3287a8,6);
                        }
                        lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
                      }
                      else {
                        lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
                      }
                    }
                    else {
                      lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
                    }
                  }
                  else {
                    lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
                  }
                }
                else {
                  lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
                }
              }
              else {
                lVar2 = (long)(char)(*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
              }
            }
            lVar7 = (long)((int)lVar7 + 1);
            pfVar4 = pfVar4 + 0x20;  // T* element step
          } while (lVar7 < lVar2);
        }
      }
      uVar6 = uVar6 + 1;
    } while ((int)uVar6 < 0x16);
  }
  return 0;
}


RT_GUEST_FN(0x00259138, guest::SpawnDoorMenuEntity)

}  // namespace guest
