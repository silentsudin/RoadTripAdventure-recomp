// guest:0x00208a88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Pseudocode carries the GIF register payloads as undefined8; guest_decls truncates them.

uint32_t titlePressStart(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  uint uVar1;
  undefined8 uVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80ffffff;
    gifBeginSpriteBatch(0x177a9e0,0x20043dce69329d8eULL);
    gifSetSpriteCount(0x177aa40,1);
    uiSpriteWriteFull(0x177aa50,0,0,0,0,0x280,0x1c0);
    (*(uint32_t*)rtguest::G(0x177aa94)) = 0xffff;
    gifSetTexRegs(0x177aaa0,0x20047b4e253221f2ULL,0);
    gifSetSpriteCount(0x177aac0,1);
    uiSpriteWrite(0x177aad0,0x10,0x18,0,0,0x1b8,0xdc);
    gifChainEndTag(0x177a9d0,0x14);
  }
  sendTexturePackets(0x177a9d0);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  if ((int)uVar1 < 0x10) {
    uiFade((0xf - uVar1) * 0x8000000);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  }
  if ((int)uVar1 < 0x708) {
    if ((uVar1 & 0x20) == 0) goto LAB_00208c4c;
    if ((*(uint32_t*)rtguest::G(0x333284)) == 1) {
      uVar2 = 0x2ea0a8;
    }
    else if ((*(uint32_t*)rtguest::G(0x333284)) < 2) {
      if ((*(uint32_t*)rtguest::G(0x333284)) != 0) goto LAB_00208c4c;
      uVar2 = 0x2ea090;
    }
    else {
      if ((*(uint32_t*)rtguest::G(0x333284)) != 2) goto LAB_00208c4c;
      uVar2 = 0x2ea0c8;
    }
    uiDrawTextCell(10,0xb,uVar2,4);
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
  }
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
LAB_00208c4c:
  if (uVar1 == 0x718) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
    gs_tasks_kill(param_1);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
  }
  return 0;
}


RT_GUEST_FN(0x00208a88, guest::titlePressStart)

}  // namespace guest
