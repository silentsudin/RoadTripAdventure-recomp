// guest:0x0023a8e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RenderEscapeText(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  uint uVar1;
  byte bVar2;
uint32_t pbVar3;
  undefined8 uVar4;
  int iVar5;
  long lVar6;
  int iVar7;
  int iVar8;
uint32_t auStack_70 = rtguest::allocFrame(32);
  
  lVar6 = 0;
  uVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) / 0xc;
  iVar7 = uVar1 - 1;
  if (uVar1 != 0) {
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)param_2));
    do {
      if (0x1f < bVar2) {
        do {
          lVar6 = (long)((int)lVar6 + 1);
          param_2 = param_2 + ((int)((uint32_t)bVar2 << 0x18) < 0) + 1;
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)param_2));
          if (bVar2 < 0x20) goto LAB_0023a988;
        } while( true );
      }
      do {
        param_2 = param_2 + 1;
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)param_2));
        if (bVar2 == 10) break;
LAB_0023a988:
      } while (bVar2 < 0x20);
      iVar7 = iVar7 + -1;
    } while (iVar7 != -1);
  }
  iVar8 = 0;
  uiSetPaletteBank(10);
  uVar1 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) % 0xc;
  iVar7 = uVar1 - 4;
  iVar5 = 0xad - uVar1;
  if (iVar7 < 0) {
    iVar7 = 0;
  }
  do {
    uVar4 = 0x46;
    bVar2 = (*(int8_t*)rtguest::G((uint32_t)param_2));
    while (0x1f < bVar2) {
      if (lVar6 == (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2)))) {
      }
      if (bVar2 != 0x24) {
        uVar4 = uiDrawGlyph(uVar4,(iVar5 + iVar7) * 2,iVar7 << 1,param_2);
        pbVar3 = param_2;
        goto switchD_0023aa48_caseD_42;
      }
      pbVar3 = param_2 + 1;
      switch((*(int8_t*)rtguest::G((uint32_t)pbVar3))) {
      case 0x41:
        pbVar3 = param_2 + 2;
        lVar6 = (long)((int)lVar6 + 1);
        ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333918)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fcf8 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4 + 4))) / 1000);
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,auStack_70);
        break;
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x48:
      case 0x49:
      case 0x4a:
      case 0x4b:
      case 0x4c:
      case 0x4d:
      case 0x4f:
      case 0x50:
      case 0x51:
        break;
      case 0x47:
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fda6);
        break;
      case 0x4e:
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fd8c);
        break;
      case 0x52:
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,
                                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10)))))) * 4)));
        break;
      case 0x53:
        uVar4 = uiDrawNumberAuto(uVar4,(iVar5 + iVar7) * 2,(*(uint32_t*)rtguest::G(0x1782a6c)),10);
        break;
      case 0x54:
        pbVar3 = param_2 + 2;
        if ((*(int8_t*)rtguest::G((uint32_t)pbVar3)) == 0x37) {
          ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333920)), (*(uint32_t*)rtguest::G(0x1780a10)) / 9000, ((*(uint32_t*)rtguest::G(0x1780a10)) % 9000) / 0x96);
        }
        else {
          ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333920)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787008 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4))) / 9000, ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787008 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4))) % 9000) / 0x96, 0x96, 9000);
        }
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,auStack_70);
        break;
      case 0x55:
        uVar4 = uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fd9f);
        break;
      default:
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
        goto LAB_0023ac4c;
      }
switchD_0023aa48_caseD_42:
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar3));
LAB_0023ac4c:
      param_2 = pbVar3 + ((char)bVar2 < '\0') + 1;
      lVar6 = (long)((int)lVar6 + 1);
      bVar2 = (*(int8_t*)rtguest::G((uint32_t)param_2));
    }
    if (bVar2 != 10) {
    }
    iVar8 = iVar8 + 1;
    param_2 = param_2 + 1;
    iVar5 = iVar5 + 0xc;
    iVar7 = 0;
    if (2 < iVar8) {
      if ((((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) == 0) || (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) / 0xc) % 3 == 0)) &&
         ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) == 0x7fff)) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = (short)lVar6;
      }
      return (uint32_t)0x0;
    }
  } while( true );
}


RT_GUEST_FN(0x0023a8e8, guest::RenderEscapeText)

}  // namespace guest
