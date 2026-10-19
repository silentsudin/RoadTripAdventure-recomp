// guest:0x002560f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandlePaintEditorInput(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  char cVar2;
  uint uVar3;
  ushort uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  long lVar8;
  undefined1 uVar9;
  
  uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
  if (((uVar4 & 0x1000) != 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) != '\0')) {
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + -1;
    PlayUiSound(0x1d);
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
  }
  if ((uVar4 & 0x4000) == 0) {
    uVar9 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  }
  else if ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) < 8) {  // pseudo byte = unsigned
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + 1;
    PlayUiSound(0x1d);
    uVar4 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x34)));
    uVar9 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  }
  else {
    uVar9 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
  }
  lVar8 = 0;
  if ((uVar4 & 0x8000) != 0) {
    lVar8 = -1;
  }
  if ((uVar4 & 0x2000) != 0) {
    lVar8 = 1;
  }
  iVar7 = (int)lVar8;
  switch(uVar9) {
  case 0:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0xa000) != 0) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) ^ 1;
LAB_00256464:
      PlayUiSound(0x1d);
    }
    goto switchD_002561a0_default;
  case 1:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar5 = uVar3 & 0xf;
    if ((lVar8 < 1) || (0xe < uVar5)) {
      uVar1 = 0xfffffff0;
      if (lVar8 < 0) {
        uVar6 = uVar3 & 0xf;
        if (uVar5 == 0) break;
        goto LAB_002561f4;
      }
    }
    else {
LAB_002561f4:
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar5 = uVar5 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xfffffff0;
    uVar6 = uVar5 & 0xf;
    break;
  case 2:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar6 = uVar3 >> 4 & 0xf;
    if (((0 < lVar8) && (uVar6 < 0xf)) || ((lVar8 < 0 && (uVar6 != 0)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar6 = uVar6 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xffffff0f;
    uVar6 = (uVar6 & 0xf) << 4;
    break;
  case 3:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar6 = uVar3 >> 8 & 0xf;
    if (((0 < lVar8) && (uVar6 < 0xf)) || ((lVar8 < 0 && (uVar6 != 0)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar6 = uVar6 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xfffff0ff;
    uVar6 = (uVar6 & 0xf) << 8;
    break;
  case 4:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 7;
      cVar2 = '\0';
      goto LAB_00256470;
    }
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar6 = uVar3 >> 0xc & 0xf;
    if (((0 < lVar8) && (uVar6 < 0xf)) || ((lVar8 < 0 && (uVar6 != 0)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar6 = uVar6 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xffff0fff;
    uVar6 = (uVar6 & 0xf) << 0xc;
    break;
  case 5:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar6 = uVar3 >> 0x10 & 0xf;
    if (((0 < lVar8) && (uVar6 < 0xf)) || ((lVar8 < 0 && (uVar6 != 0)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar6 = uVar6 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xfff0ffff;
    uVar6 = (uVar6 & 0xf) << 0x10;
    break;
  case 6:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27))) == '\0') {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 3;
      cVar2 = '\0';
      goto LAB_00256470;
    }
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    uVar6 = uVar3 >> 0x14 & 0xf;
    if (((0 < lVar8) && (uVar6 < 0xf)) || ((lVar8 < 0 && (uVar6 != 0)))) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = 0x20;
      uVar6 = uVar6 + iVar7;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    }
    uVar1 = 0xff0fffff;
    uVar6 = (uVar6 & 0xf) << 0x14;
    break;
  case 7:
    uVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 5))) + iVar7;  // pseudo (uint)*(byte*)
    if (0xb < uVar3) {
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
      goto LAB_00256470;
    }
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 5))) = (char)uVar3;
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 3))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 3))) + (char)lVar8;
    if (lVar8 != 0) {
      PlayUiSound(0x1d);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = 1;
    }
    goto switchD_002561a0_default;
  case 8:
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x30))) & 0xa000) != 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25))) ^ 1;
      goto LAB_00256464;
    }
  default:
    goto switchD_002561a0_default;
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_3)) = uVar3 & uVar1 | uVar6;
switchD_002561a0_default:
  cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x27)));
LAB_00256470:
  if (cVar2 == '\0') {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_3));
    (*(uint32_t*)rtguest::G((uint32_t)param_3)) = uVar3 & 0xff000fff | (uVar3 & 0xf) << 0xc | ((uVar3 & 0xf0) >> 4) << 0x10 |
               ((uVar3 & 0xf00) >> 8) << 0x14;
  }
  return 0;
}


RT_GUEST_FN(0x002560f0, guest::HandlePaintEditorInput)

}  // namespace guest
