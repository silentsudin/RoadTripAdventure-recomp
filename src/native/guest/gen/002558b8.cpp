// guest:0x002558b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawPaintEditorDigits(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t abStack_70 = rtguest::allocFrame(16);
    uint32_t iStack_60 = rtguest::allocFrame(4);
  byte bVar1;
  int iVar2;
  uint uVar3;
  undefined1 uVar4;
  undefined8 uVar5;
  int iVar6;
  int iVar7;
uint32_t puVar8;
  int iVar9;
  uint uVar10;
uint32_t puVar11;
uint32_t puVar12;
uint32_t puVar13;
uint32_t puVar14;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_60)) = param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) == '\0') {
    uiSpriteWrite(param_2 + 0x6c0,0x6c,0x3e,0,0xf8,0x40,0x18);
    uiSpriteWrite(param_2 + 0x710,0xcc,0x3e,0x40,0xe0,0x40,0x18);
  }
  else {
    uiSpriteWrite(param_2 + 0x6c0,0xcc,0x3e,0x40,0xf8,0x40,0x18);
    uiSpriteWrite(param_2 + 0x710,0x6c,0x3e,0,0xe0,0x40,0x18);
  }
  uVar5 = 0x80808080;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) == '\0') {
    uVar5 = 0x60404040;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x8a0))) = uVar5;
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 5)));
  uiSpriteWrite(param_2 + 0x800,
                ((int)((uint32_t)bVar1 * 0x1c - (uint32_t)bVar1) / 2 + 0x65) * 0x10000 >> 0x10,
                (bVar1 & 1) * 0x120000 + 0x13e0000 >> 0x10,0x1b0,0x120,0x20,0x1c);
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 5)));
  uiSpriteWrite(param_2 + 0x850,
                ((int)((uint32_t)bVar1 * 0x1c - (uint32_t)bVar1) / 2 + 0x65) * 0x10000 >> 0x10,
                (bVar1 & 1) * 0x120000 + 0x1260000 >> 0x10,0x1d5,0x11c,0x20,0x20);
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x26))) != '\a') {
    uiHideSprite(param_2 + 0x850);
  }
  iVar9 = 0;
  iVar6 = 0x9a0;
  do {
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8 + iVar6))) = 1;
    uVar5 = 0x80808080;
    if ((iVar9 != (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x26))) - 1) &&
       ((iVar9 < 3 || (uVar5 = 0x40404040, (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) != '\0')))) {
      uVar5 = 0x80404040;
    }
    iVar9 = iVar9 + 1;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + iVar6))) = uVar5;
    iVar6 = iVar6 + 0x60;
  } while (iVar9 < 6);
  uVar10 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
  iVar6 = 0;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(0) * 1)) = (uint32_t)uVar10 & 0xf;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(1) * 1)) = (uint32_t)(uVar10 >> 4) & 0xf;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(2) * 1)) = (uint32_t)(uVar10 >> 8) & 0xf;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(3) * 1)) = (uint32_t)(uVar10 >> 0xc) & 0xf;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(4) * 1)) = (uint32_t)(uVar10 >> 0x10) & 0xf;
  iVar9 = 0;
  (*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(5) * 1)) = (uint32_t)(uVar10 >> 0x14) & 0xf;
  do {
    uVar10 = 0;
    iVar2 = (iVar9 + iVar6) * 0x200;
    iVar7 = iVar2 + param_2;
    puVar14 = (uint32_t)(iVar2 + param_2 + 8 + 0xbe0);
    puVar8 = (uint32_t)(iVar7 + 0xbe2);
    puVar13 = (uint32_t)(iVar7 + 0xbe3);
    puVar12 = (uint32_t)(iVar7 + 0xbe0);
    puVar11 = (uint32_t)(iVar7 + 0xbe1);
    do {
      (*(uint64_t*)rtguest::G((uint32_t)puVar14)) = 1;
      uVar3 = uVar10;
      if ((iVar6 != 0) && (iVar6 != 3)) {
        uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)((iVar6 / 3) * 3) * 1));
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar12)) = (*(uint32_t*)rtguest::G((uint32_t)0x29d190 + (uint32_t)(uVar3) * 4));
      uVar3 = uVar10;
      if ((iVar6 != 1) && (iVar6 != 4)) {
        uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)((iVar6 / 3) * 3 + 1) * 1));
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = (*(uint32_t*)rtguest::G((uint32_t)0x29d190 + (uint32_t)(uVar3) * 4));
      uVar3 = uVar10;
      if ((iVar6 != 2) && (iVar6 != 5)) {
        uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)((iVar6 / 3) * 3 + 2) * 1));
      }
      (*(uint8_t*)rtguest::G((uint32_t)puVar8)) = (*(uint32_t*)rtguest::G((uint32_t)0x29d190 + (uint32_t)(uVar3) * 4));
      if ((iVar6 < 3) || (uVar4 = 0x40, (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) != '\0')) {
        uVar4 = 0x80;
      }
      uVar10 = uVar10 + 1;
      (*(uint8_t*)rtguest::G((uint32_t)puVar13)) = uVar4;
      puVar13 = puVar13 + 0x60;
      puVar8 = puVar8 + 0x60;
      puVar11 = puVar11 + 0x60;
      puVar12 = puVar12 + 0x60;
      puVar14 = puVar14 + 0xc;
    } while ((int)uVar10 < 0x10);
    iVar6 = iVar6 + 1;
    iVar9 = iVar9 + 2;
  } while (iVar6 < 6);
  uiSetSpriteXY(param_2 + 0x3000,param_2 + 0x3010,0x70,0x8f,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(0) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3014))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3018))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x2ff8))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3008))) = 5;
  uiSetSpriteXY(param_2 + 0x3030,param_2 + 0x3040,0x70,0xa2,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(1) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3044))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3048))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3028))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3038))) = 5;
  uiSetSpriteXY(param_2 + 0x3060,param_2 + 0x3070,0x70,0xb5,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(2) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3074))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3078))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3058))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3068))) = 5;
  uiSetSpriteXY(param_2 + 0x3090,param_2 + 0x30a0,0x70,0xdd,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(3) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30a4))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30a8))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3088))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3098))) = 5;
  uiSetSpriteXY(param_2 + 0x30c0,param_2 + 0x30d0,0x70,0xf0,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(4) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30d4))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30d8))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30b8))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30c8))) = 5;
  uiSetSpriteXY(param_2 + 0x30f0,param_2 + 0x3100,0x70,0x103,
                (uint32_t)(0x000a0000 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(5) * 1)) * 0xa0000) >> 0x10,2);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3104))) = 0xfffffe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3108))) = 5;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30e8))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30f8))) = 5;
  uVar4 = 0x80;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x27))) == '\0') {
    uVar4 = 0x40;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3083))) = uVar4;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30e3))) = uVar4;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30b3))) = uVar4;
  uiDrawNumber10(0x128,0x81,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(0) * 1)),0);
  uiDrawNumber10(0x128,0x94,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(1) * 1)),0);
  uiDrawNumber10(0x128,0xa7,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(2) * 1)),0);
  uiDrawNumber10(0x128,0xcf,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(3) * 1)),0);
  uiDrawNumber10(0x128,0xe2,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(4) * 1)),0);
  uiDrawNumber10(0x128,0xf5,(*(int8_t*)rtguest::G((uint32_t)abStack_70 + (uint32_t)(5) * 1)),0);
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x26)))) {
  case 0:
    uiDrawArrow(0x20,0x3e);
    break;
  case 1:
    uiDrawArrow(0x20,0x8a);
    break;
  case 2:
    uiDrawArrow(0x20,0x92);
    break;
  case 3:
    uiDrawArrow(0x20,0x9a);
    break;
  case 4:
    uiDrawArrow(0x20,0xd8);
    break;
  case 5:
    uiDrawArrow(0x20,0xe0);
    break;
  case 6:
    uiDrawArrow(0x20,0xe8);
    break;
  case 7:
    uiDrawArrow(0x20,0x148);
    break;
  case 8:
    uiDrawArrow(0x20,0x18a);
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_60)) + 0x25))) == '\0') {
    uiSpriteWrite(param_2 + 0x760,0x51,0x18a,0xb0,0x148,0x60,0x18);
    uiSpriteWrite(param_2 + 0x7b0,0xba,0x18a,0x50,0x160,0x60,0x18);
  }
  else {
    uiSpriteWrite(param_2 + 0x760,0x51,0x18a,0xb0,0x160,0x60,0x18);
    uiSpriteWrite(param_2 + 0x7b0,0xba,0x18a,0xb0,0x130,0x60,0x18);
  }
  return 0;
}


RT_GUEST_FN(0x002558b8, guest::DrawPaintEditorDigits)

}  // namespace guest
