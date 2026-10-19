// guest:0x002017b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawGlyph(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
    uint32_t uStack_40 = rtguest::allocFrame(2);
  ushort uVar1;
  undefined2 uVar2;
uint32_t pbVar3;
  uint uVar4;
  byte bVar5;
  int iVar6;
uint32_t asStack_3e = rtguest::allocFrame(14);
  
  iVar6 = (int)param_3;
  uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)param_4));
  pbVar3 = param_4 + 1;
  uVar2 = (uint32_t)param_1;
  if (0x7f < uVar4) {
    uVar1 = CONCAT11((*(int8_t*)rtguest::G((uint32_t)param_4)),(*(int8_t*)rtguest::G((uint32_t)pbVar3)));
    if (uVar1 == 0x8140) {
      return param_1 + 10;
    }
    if (uVar1 == 0x818b) {
      if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
        uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,uVar2,(short)param_2,0xc0,0x60,0x10,
                      (0x18 - iVar6) * 0x10000 >> 0x10);
        (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
      }
      return param_1 + 0x10;
    }
    if (uVar1 < 0x889f) {
      uiKanaGlyphUV(uStack_40,asStack_3e,uVar1,iVar6);
    }
    else {
      uiKanjiGlyphUV(uStack_40,asStack_3e,uVar1,iVar6);
    }
    if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
      uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,uVar2,(param_2 + 2) * 0x10000 >> 0x10,
                    (*(uint16_t*)rtguest::G((uint32_t)uStack_40)),(*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)),0x14,(0x14 - iVar6) * 0x10000 >> 0x10);
      (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
    }
    return param_1 + 0x14;
  }
  if (uVar4 == 0x20) {
    return param_1 + 8;
  }
  if (uVar4 == 0x2f) {
    if ((*(int8_t*)rtguest::G((uint32_t)pbVar3)) == 0x65) {
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x60;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      uVar4 = 0x65;
    }
    else {
      uVar4 = 0x2f;
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0xe0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0;
    }
    goto LAB_00201a74;
  }
  if (uVar4 == 0x60) {
    uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
    (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x70;
    if (uVar4 == 0x65) {
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)pbVar3)) < 0x66) {
      if (uVar4 == 0x61) {
        (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1e0;
        (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      }
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1b0;
    }
    else {
      if (uVar4 == 0x75) {
        (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x180;
        (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      }
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1b0;
    }
    goto switchD_002018dc_default;
  }
  if (uVar4 == 0x5e) {
    uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
    (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1b0;
    switch(uVar4) {
    case 0x61:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x62:
    case 99:
    case 100:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
switchD_002018dc_caseD_62:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1b0;
    default:
switchD_002018dc_default:
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x60;
      break;
    case 0x65:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x80;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x69:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0xc0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x6f:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x120;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x75:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 400;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
    }
  }
  else {
    if (uVar4 != 0x7e) {
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = (uint32_t)((int)(uVar4 - 0x21) % 0x1f << 4);
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = (short)((int)(uVar4 - 0x21) / 0x1f) * 0x18;
    }
    uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
    (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1b0;
    switch(uVar4) {
    case 0x41:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x10;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      break;
    case 0x42:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x50:
    case 0x51:
    case 0x53:
    case 0x54:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x62:
    case 100:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x74:
      goto switchD_002018dc_caseD_62;
    case 0x43:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x40;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      break;
    case 0x4f:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x130;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      break;
    case 0x52:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0xa0;
    case 0x55:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 400;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      break;
    case 0x61:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x20;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 99:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x50;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x65:
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x90;
      break;
    case 0x69:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0xd0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x6f:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x130;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    case 0x73:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1c0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x78;
      break;
    case 0x75:
      (*(uint16_t*)rtguest::G((uint32_t)uStack_40)) = 0x1a0;
      (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)) = 0x90;
      break;
    default:
    }
  }
LAB_00201a74:
  bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar4 + 0x29a860)));
  if ((*(uint32_t*)rtguest::G(0x335684)) < 0x9fc) {
    uiSpriteWrite((*(uint32_t*)rtguest::G(0x335680)) + (*(uint32_t*)rtguest::G(0x335684)) * 0x10 + 0x70,uVar2,(short)param_2,
                  (int)(((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)uStack_40)) + (0x10 - (char)bVar5) / 2) * 0x10000) >> 0x10,
                  (*(int16_t*)rtguest::G((uint32_t)asStack_3e + (uint32_t)(0) * 2)),
                  (int)((((int)(char)bVar5 + (uint32_t)(bVar5 >> 7) >> 1) + 8) * 0x10000) >> 0x10,
                  (0x18 - iVar6) * 0x10000 >> 0x10);
    bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar4 + 0x29a860)));
    (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 5;
  }
  return param_1 + (char)bVar5;
}


RT_GUEST_FN(0x002017b0, guest::uiDrawGlyph)

}  // namespace guest
