// guest:0x00242fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t townMapTask(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t fStack_98 = rtguest::allocFrame(4);
    uint32_t fStack_e8 = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    uint32_t uStack_e0 = rtguest::allocFrame(4);
  char cVar1;
  ushort uVar2;
  uint uVar3;
  long lVar4;
  int iVar5;
uint32_t psVar6;
  int iVar7;
  int iVar8;
  float fVar9;
  float fVar10;
uint32_t afStack_110 = rtguest::allocFrame(40);
uint32_t auStack_d0 = rtguest::allocFrame(8);
uint32_t afStack_c0 = rtguest::allocFrame(40);
uint32_t auStack_80 = rtguest::allocFrame(32);
  
  iVar5 = (int)param_2;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) & 0x10000) == 0) {
    townMapBezel();
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & (*(uint16_t*)rtguest::G(0x1780a5a))) == 0) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x22)));
    }
    else {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) ^ 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x22)));
    }
    if (cVar1 == -1) {
      gs_map_draw_functions_town(0x177ace0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))),param_2);
      fVar9 = 50.0 / (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) * 0x10)));
      vifUploadColours(0x2a4370,0x2a4360);
      psVar6 = 0x0177b058;
      iVar8 = 0x270;
      iVar7 = 0x16;
      fVar10 = (float)(int)(*(uint32_t*)rtguest::G(0x177ae24)) * (*(uint32_t*)rtguest::G(0x332f38));
      vu0UnitMatrix(afStack_110);
      (*(float*)rtguest::G((uint32_t)afStack_110 + (uint32_t)(0) * 4)) = fVar9;
      (*(float*)rtguest::G((uint32_t)fStack_e8)) = fVar9;
      vu0RotMatrixY(fVar10 * 3.0517578e-05,afStack_110,afStack_110);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = (*(uint32_t*)rtguest::G(0x177ace0));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = (*(uint32_t*)rtguest::G(0x177ace8));
      vifObjectMatrix(afStack_110);
      vifCallModel(0x290870);
      vifUploadColours(0x2a4390,0x2a4360);
      do {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar6 + 0xaf))) * 8)));
        lVar4 = tileSlotLoaded(param_2,uVar2);
        if (((lVar4 != 0) && ((*(int16_t*)rtguest::G((uint32_t)psVar6)) != 0)) &&
           (lVar4 = tileSlotOrigin(auStack_d0,(int)0x177ace0 + iVar8,uVar2 & 3),
           lVar4 != 0)) {
          vu0UnitMatrix(afStack_c0);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_d0 + (uint32_t)(0) * 4));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_c8));
          (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(0) * 4)) = fVar9;
          (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar9;
          vifObjectMatrix(afStack_c0);
          vifCallModel(0x290940);
        }
        iVar7 = iVar7 + -1;
        psVar6 = psVar6 + 0x138;
        iVar8 = iVar8 + 0x270;
      } while (-1 < iVar7);
      lVar4 = GetFlaggedSlotCount(0xf,0x2d,0);
      if (lVar4 == 0) {
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      }
      else {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) != 0) {
          vifUploadColours(0x2a43a0,0x2a4360);
          townMapCoins(fVar9,param_2);
        }
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      }
      if ((uVar3 & 0x1000) != 0) {
        uiDrawText(0xe6,0x11,0x2eca30,0);
        uiDrawText(0x1e,0x1d,0x2eca48,0);
        uiDrawText(0x1e,0x29,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be438 + (uint32_t)((*(int32_t*)rtguest::G(0x335734)) * 2) * 4)),0);
        uiDrawText(0x212,0x11,0x333930,0);
        ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x2eca58)), (*(int32_t*)rtguest::G(0x335730)) / 9000, ((*(int32_t*)rtguest::G(0x335730)) % 9000) / 0x96);
        uiDrawText(0x212,0x1d,auStack_80,0);
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x00242fe8, guest::townMapTask)

}  // namespace guest
