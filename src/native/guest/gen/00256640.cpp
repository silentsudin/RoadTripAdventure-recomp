// guest:0x00256640 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PaintEditorEditTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  ushort uVar1;
uint32_t pcVar2;
  uint uVar3;
  int iVar4;
uint32_t pcVar5;
  int iVar6;
uint32_t puVar7;
uint32_t aiStack_40 = rtguest::allocFrame(16);
  
  iVar6 = (int)param_1;
  puVar7 = (uint32_t)((int)0x177f760 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x1c))) * 0x1c);
  HandlePaintEditorInput(param_1,param_2,puVar7);
  DrawPaintEditorDigits(param_1,0x175b570,puVar7);
  (*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(2) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(1) * 4)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(0) * 4)) = 0;
  if ((uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) - 1U) < 6) {
    (*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)((int)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) - 1) % 3) * 4)) = 0xff;
  }
  (*(uint64_t*)rtguest::G(0x175e6b8)) = (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(0) * 4)) | (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) << 0x1a |
                 (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(2) * 4)) << 0x10 | (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_40 + (uint32_t)(1) * 4)) << 8;
  if ((long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) != 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) + -1;
  }
  uiSetQuadXY(0x175e6c0,((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) & 0x1f) << 2,0,0x80,0x80);
  DrawPaintCursorSprites(param_1,0x175b570);
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x30)));
  if ((((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x26))) != '\b') || ((uVar1 & 0x40) == 0)) && ((uVar1 & 0x10) == 0))
  goto LAB_00256870;
  if ((uVar1 & 0x10) != 0) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
  }
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20))) == (*(uint32_t*)rtguest::G((uint32_t)puVar7))) {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    pcVar2 = 0x2568d8;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 4))) = 0xffffffff;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x24))) = 1;
  }
  else {
    PlayUiSound(0x1a);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) == '\0') {
      pcVar2 = 0x23bb88;
      pcVar5 = 0x334530u;
LAB_00256820:
      gs_tasks_spawn(param_1,pcVar2,pcVar5);
    }
    else {
      iVar4 = 100;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar7)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
      if ((uVar3 & 0xffffff) == 0) {
        iVar4 = 0;
      }
      if (uVar3 >> 0x18 != 0) {
        iVar4 = iVar4 + 100;
      }
      if ((*(uint32_t*)rtguest::G(0x177fdb4)) < iVar4) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x25))) = 0;
        pcVar2 = 0x23bb00;
        pcVar5 = 0x328448u;
        goto LAB_00256820;
      }
      gs_tasks_spawn(param_1,0x23bb88,0x334538);
    }
    pcVar2 = 0x2564e8;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0xc))) = pcVar2;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) = 0;
LAB_00256870:
  vifDirect(0x175b570,0x32c);
  iVar4 = 100;
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)puVar7)) ^ (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x20)));
  if ((uVar3 & 0xffffff) == 0) {
    iVar4 = 0;
  }
  iVar6 = iVar4 + 100;
  if (uVar3 >> 0x18 == 0) {
    iVar6 = iVar4;
  }
  DrawScoreAndRemaining(iVar6,param_2);
  return 0;
}


RT_GUEST_FN(0x00256640, guest::PaintEditorEditTask)

}  // namespace guest
