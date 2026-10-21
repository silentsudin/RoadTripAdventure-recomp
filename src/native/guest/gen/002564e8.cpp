// guest:0x002564e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PaintEditorConfirmTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x0000000001760000ull;  // walk const
  }
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
uint32_t puVar6;
  
  iVar5 = (int)param_1;
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) * 0x1c;
  puVar6 = (uint32_t)((int)0x177f760 + iVar3);
  RT_CALLRA(0x25652c, guest::DrawPaintEditorDigits(param_1,0x175b570,puVar6));
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) != '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) == 0) {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x25))) == '\0') {
        uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x23)));
        (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f760 + iVar3 + 5))) = uVar1;
      }
      else {
        iVar3 = 100;
        if ((((*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)))) & 0xffffff) == 0) {
          iVar3 = 0;
        }
        iVar4 = iVar3 + 100;
        if (((*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)))) >> 0x18 == 0) {
          iVar4 = iVar3;
        }
        RT_CALLRA(0x256584, guest::SpendTimerBudget(iVar4,0));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20))) = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0xffffffff;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 0x2568d8;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 0x256640;
      RT_CALLRA(0x2565d8, guest::PlayUiSound(0x1b));
    }
  }
  RT_CALLRA(0x2565e4, guest::vifDirect(0x175b570,0x32c));
  iVar3 = 100;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)puVar6)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x20)));
  if ((uVar2 & 0xffffff) == 0) {
    iVar3 = 0;
  }
  iVar5 = iVar3 + 100;
  if (uVar2 >> 0x18 == 0) {
    iVar5 = iVar3;
  }
  DrawScoreAndRemaining(iVar5,param_2);
  return 0;
}


RT_GUEST_FN(0x002564e8, guest::PaintEditorConfirmTask)

}  // namespace guest
