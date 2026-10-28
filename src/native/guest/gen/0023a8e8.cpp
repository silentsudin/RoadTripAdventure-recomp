// guest:0x0023a8e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RenderEscapeText(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x58, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x28, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x38, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[4] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(7, (uint32_t)param_3);  // walk: = param_3
  }
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
  uVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) / 0xc;
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
LAB_0023a988: ;
      } while (bVar2 < 0x20);
      iVar7 = iVar7 + -1;
    } while (iVar7 != -1);
  }
  iVar8 = 0;
  RT_CALLRA(0x23a9bc, guest::uiSetPaletteBank(10));
  uVar1 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) % 0xc;
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
        return param_2;
      }
      if (bVar2 != 0x24) {
        uVar4 = RT_CALLRA(0x23ac3c, guest::uiDrawGlyph(uVar4,(iVar5 + iVar7) * 2,iVar7 << 1,param_2));
        pbVar3 = param_2;
        goto switchD_0023aa48_caseD_42;
      }
      pbVar3 = param_2 + 1;
      switch((*(int8_t*)rtguest::G((uint32_t)pbVar3))) {
      case 0x41:
        pbVar3 = param_2 + 2;
        lVar6 = (long)((int)lVar6 + 1);
        ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333918)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177fcf8 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4 + 4))) / 1000);
        uVar4 = RT_CALLRA(0x23aa70, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,auStack_70));
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
        uVar4 = RT_CALLRA(0x23aa90, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fda6));
        break;
      case 0x4e:
        uVar4 = RT_CALLRA(0x23aab0, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fd8c));
        break;
      case 0x52:
        uVar4 = RT_CALLRA(0x23aad0, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,
                                (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x2be398 + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0x10)))))) * 4))));
        break;
      case 0x53:
        uVar4 = RT_CALLRA(0x23aaf4, guest::uiDrawNumberAuto(uVar4,(iVar5 + iVar7) * 2,(*(uint32_t*)rtguest::G(0x1782a6c)),10));
        break;
      case 0x54:
        pbVar3 = param_2 + 2;
        if ((*(int8_t*)rtguest::G((uint32_t)pbVar3)) == 0x37) {
          ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333920)), (*(uint32_t*)rtguest::G(0x1780a10)) / 9000, ((*(uint32_t*)rtguest::G(0x1780a10)) % 9000) / 0x96);
        }
        else {
          ::sprintf((char *)rtguest::G((uint32_t)(auStack_70)), (char *)rtguest::G((uint32_t)(0x333920)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787008 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4))) / 9000, ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x01787008 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3)) * 4))) % 9000) / 0x96, 0x96, 9000);
        }
        uVar4 = RT_CALLRA(0x23ab54, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,auStack_70));
        break;
      case 0x55:
        uVar4 = RT_CALLRA(0x23ac1c, guest::uiDrawTextShade(uVar4,iVar5 + iVar7,iVar7,0x177fd9f));
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
      return param_2;
    }
    iVar8 = iVar8 + 1;
    param_2 = param_2 + 1;
    iVar5 = iVar5 + 0xc;
    iVar7 = 0;
    if (2 < iVar8) {
      if ((((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) == 0) || (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) / 0xc) % 3 == 0)) &&
         ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) == 0x7fff)) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 2))) = (short)lVar6;
      }
      return (uint32_t)0x0;
    }
  } while( true );
}


RT_GUEST_FN(0x0023a8e8, guest::RenderEscapeText)

}  // namespace guest
