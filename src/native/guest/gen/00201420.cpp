// guest:0x00201420 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawGlyphScaled(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(112);
    uint32_t uStack_70 = rtguest::allocFrame(2);
uint32_t auStack_6e = rtguest::allocFrame(14);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  char cVar1;
  char cVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
uint32_t pcVar10;
  int iVar11;
  
  iVar7 = (int)param_4;
  uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_5));
  if (uVar6 < 0x80) {
    if (uVar6 == 0x20) {
      param_6 = param_6 * 10;
    }
    else {
      pcVar10 = (uint32_t)(uVar6 + 0x29a860);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar10));
      iVar8 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = (int)param_3;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar10));
      iVar7 = (0x18 - iVar7) * 0x10000 >> 0x10;
      iVar11 = ((int)(uVar6 - 0x21) % 0x1f) * 0x10 + (0x10 - cVar1) / 2;
      iVar9 = ((int)(uVar6 - 0x21) / 0x1f) * 0x180000 >> 0x10;
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        iVar4 = cVar1 * param_6;
        iVar5 = iVar7 * param_6;
        if (iVar4 < 0) {
          iVar4 = iVar4 + 0xff;
        }
        if (iVar5 < 0) {
          iVar5 = iVar5 + 0xff;
        }
        uiSetSpriteXY(iVar8 + 0x90,iVar8 + 0xb0,(short)param_1,param_2,(iVar4 << 8) >> 0x10,
                      (iVar5 << 8) >> 0x10);
        uiSetSpriteUV(iVar8 + 0x80,iVar8 + 0xa0,iVar11,iVar9,cVar1,iVar7);
        uiSetSpriteClamp(iVar8 + 0x70,iVar11,iVar9,cVar1,iVar7);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xb4))) = (*(int32_t*)rtguest::G((uint32_t)iStack_60));
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x78))) = 8;
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa8))) = 3;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xb8))) = 5;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x88))) = 3;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x98))) = 5;
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)pcVar10));
      }
      param_6 = cVar2 * param_6;
    }
  }
  else {
    uVar3 = CONCAT11((*(int8_t*)rtguest::G((uint32_t)param_5)),(*(int8_t*)rtguest::G((uint32_t)param_5 + (uint32_t)(1) * sizeof(int8_t))));
    if (uVar3 == 0x8140) {
      param_6 = param_6 * 10;
    }
    else {
      if (uVar3 < 0x889f) {
        uiKanaGlyphUV(uStack_70,auStack_6e,uVar3,iVar7);
      }
      else {
        uiKanjiGlyphUV(uStack_70,auStack_6e,uVar3,iVar7);
      }
      iVar8 = (*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
      iVar7 = (0x14 - iVar7) * 0x10000 >> 0x10;
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        iVar9 = param_6 * 0x14;
        iVar11 = iVar7 * param_6;
        if (iVar9 < 0) {
          iVar9 = iVar9 + 0xff;
        }
        if (iVar11 < 0) {
          iVar11 = iVar11 + 0xff;
        }
        uiSetSpriteXY(iVar8 + 0x90,iVar8 + 0xb0,(short)param_1,param_2,(iVar9 << 8) >> 0x10,
                      (iVar11 << 8) >> 0x10);
        uiSetSpriteUV(iVar8 + 0x80,iVar8 + 0xa0,(*(uint16_t*)rtguest::G((uint32_t)uStack_70)),(*(uint16_t*)rtguest::G((uint32_t)auStack_6e + (uint32_t)(0) * 2)),0x14,iVar7);
        uiSetSpriteClamp(iVar8 + 0x70,(*(uint16_t*)rtguest::G((uint32_t)uStack_70)),(*(uint16_t*)rtguest::G((uint32_t)auStack_6e + (uint32_t)(0) * 2)),0x14,iVar7);
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xb4))) = (int)param_3;
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x78))) = 8;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xa8))) = 3;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0xb8))) = 5;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x88))) = 3;
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x98))) = 5;
      }
      param_6 = param_6 * 0x14;
    }
  }
  if (param_6 < 0) {
    param_6 = param_6 + 0xff;
  }
  return param_1 + (param_6 >> 8);
}


RT_GUEST_FN(0x00201420, guest::uiDrawGlyphScaled)

}  // namespace guest
