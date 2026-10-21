// guest:0x00242fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t townMapTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(272);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xf8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xfc) = 0x00000000u;
  }
uint32_t afStack_110 = rtguest::allocFrame(40);
    uint32_t fStack_e8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_e0 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_d0 = rtguest::allocFrame(8);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t afStack_c0 = rtguest::allocFrame(40);
    uint32_t fStack_98 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_90 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_80 = rtguest::allocFrame(32);
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
  
  iVar5 = (int)param_2;
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) & 0x10000) == 0) {
    RT_CALLRA(0x243034, guest::townMapBezel());
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x30))) & (*(uint16_t*)rtguest::G(0x1780a5a))) == 0) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x22)));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) ^ 1;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x22)));
    }
    if (cVar1 == -1) {
      RT_CALLRA(0x243084, guest::gs_map_draw_functions_town(0x177ace0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))),param_2));
      fVar9 = rtguest::fdiv(50.0, (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002a42c8 + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) * 0x10))));
      RT_CALLRA(0x2430c4, guest::vifUploadColours(0x2a4370,0x2a4360));
      psVar6 = 0x0177b058;
      iVar8 = 0x270;
      iVar7 = 0x16;
      fVar10 = (float)(int)(*(int16_t*)rtguest::G(0x177ae24)) * rtguest::fbits(*(uint32_t*)rtguest::G(0x332f38));  // heading is a signed halfword: MIPS 0x2430cc lh; DAT_00332f38 float const (lwc1 -0x7bb8(gp), disasm 0x2430d4)
      RT_CALLRA(0x243100, guest::vu0UnitMatrix(afStack_110));
      (*(float*)rtguest::G((uint32_t)afStack_110 + (uint32_t)(0) * 4)) = fVar9;
      (*(float*)rtguest::G((uint32_t)fStack_e8)) = fVar9;
      RT_CALLRA(0x243118, guest::vu0RotMatrixY(fVar10 * 3.0517578e-05,afStack_110,afStack_110));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = (*(uint32_t*)rtguest::G(0x177ace0));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = (*(uint32_t*)rtguest::G(0x177ace8));
      RT_CALLRA(0x243130, guest::vifObjectMatrix(afStack_110));
      RT_CALLRA(0x24313c, guest::vifCallModel(0x290870));
      RT_CALLRA(0x24314c, guest::vifUploadColours(0x2a4390,0x2a4360));
      do {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)psVar6 + 0xaf))) * 8)));
        lVar4 = RT_CALLRA(0x243174, guest::tileSlotLoaded(param_2,uVar2));
        if (((lVar4 != 0) && ((*(int16_t*)rtguest::G((uint32_t)psVar6)) != 0)) &&
           (lVar4 = RT_CALLRA(0x243194, guest::tileSlotOrigin(auStack_d0,(int)0x177ace0 + iVar8,uVar2 & 3)),
           lVar4 != 0)) {
          RT_CALLRA(0x2431a4, guest::vu0UnitMatrix(afStack_c0));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_d0 + (uint32_t)(0) * 4));
          (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_c8));
          (*(float*)rtguest::G((uint32_t)afStack_c0 + (uint32_t)(0) * 4)) = fVar9;
          (*(float*)rtguest::G((uint32_t)fStack_98)) = fVar9;
          RT_CALLRA(0x2431c4, guest::vifObjectMatrix(afStack_c0));
          RT_CALLRA(0x2431cc, guest::vifCallModel(0x290940));
        }
        iVar7 = iVar7 + -1;
        psVar6 = psVar6 + 0x270;  // T* element step
        iVar8 = iVar8 + 0x270;
      } while (-1 < iVar7);
      lVar4 = RT_CALLRA(0x2431ec, guest::GetFlaggedSlotCount(0xf,0x2d,0));
      if (lVar4 == 0) {
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      }
      else {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) != 0) {
          RT_CALLRA(0x243214, guest::vifUploadColours(0x2a43a0,0x2a4360));
          RT_CALLRA(0x243220, guest::townMapCoins(fVar9,param_2));
        }
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      }
      if ((uVar3 & 0x1000) != 0) {
        RT_CALLRA(0x243244, guest::uiDrawText(0xe6,0x11,0x2eca30,0));
        RT_CALLRA(0x24325c, guest::uiDrawText(0x1e,0x1d,0x2eca48,0));
        RT_CALLRA(0x243284, guest::uiDrawText(0x1e,0x29,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be438 + (uint32_t)((*(int32_t*)rtguest::G(0x335734)) * 2) * 4)),0));
        RT_CALLRA(0x24329c, guest::uiDrawText(0x212,0x11,0x333930,0));
        ::sprintf((char *)rtguest::G((uint32_t)(auStack_80)), (char *)rtguest::G((uint32_t)(0x2eca58)), (*(int32_t*)rtguest::G(0x335730)) / 9000, ((*(int32_t*)rtguest::G(0x335730)) % 9000) / 0x96);
        RT_CALLRA(0x2432f4, guest::uiDrawText(0x212,0x1d,auStack_80,0));
      }
    }
  }
  return 0;
}


RT_GUEST_FN(0x00242fe8, guest::townMapTask)

}  // namespace guest
