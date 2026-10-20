// guest:0x0020b5e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020b5e8: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t loadShopSet(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  char cVar1;
  ushort uVar2;
uint32_t puVar3;
uint32_t piVar4;
  long lVar5;
  ulong uVar6;
  undefined8 uVar7;
uint32_t ppuVar8;
  int iVar9;
  
  switch((*(uint32_t*)rtguest::G(0x3356c0))) {
  case 0:
    RT_CALLRA(0x20b644, guest::cdReadEntryPart(0x29b29c,0x100000,0,0x3f000));
    goto LAB_0020b9d0;
  case 1:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20b698, guest::fatal(0x2ea490));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1f2e) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    break;
  case 2:
    ppuVar8 = 0x29afd0u;
    goto LAB_0020b9c4;
  case 3:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20b740, guest::fatal(0x2ea490));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1cc6) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    break;
  case 4:
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) == 0) {
      ppuVar8 = (uint32_t)0x29afdc;
    }
    else {
      ppuVar8 = 0x29b03cu;
    }
    goto LAB_0020b9c4;
  case 5:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      else {
        RT_CALLRA(0x20b800, guest::fatal(0x2ea490));
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)uVar2 * 0x10 + 0x10;
    }
    break;
  case 6:
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) == 0) {
      ppuVar8 = 0x29afe8u;
    }
    else {
      ppuVar8 = 0x29b048u;
    }
    lVar5 = RT_CALLRA(0x20b870, guest::cdReadEntry(ppuVar8,0xaafb80));
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea528u)), lVar5);
    if (0xaf8000 < lVar5) {
      RT_CALLRA(0x20b8b0, guest::fatal(0x2ea540,lVar5,0xaf800));
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x74))) = (int)0x00aafb80 + (int)lVar5;
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    return 0;
  case 7:
    piVar4 = 0x00aafb80;
    iVar9 = 0x40;
    (*(int32_t*)rtguest::G(0x177ab24)) = 0x00aafb80;
    do {
      iVar9 = iVar9 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = (int)0x00aafb80 + (*(int32_t*)rtguest::G((uint32_t)piVar4));
      piVar4 = piVar4 + sizeof(int32_t);
    } while (-1 < iVar9);
    ppuVar8 = 0x29b054u;
    goto LAB_0020b9c4;
  case 8:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20b960, guest::fatal(0x2ea490));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x285e) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    break;
  case 9:
    ppuVar8 = 0x29b150u;
LAB_0020b9c4:
    RT_CALLRA(0x20b9c4, guest::cdReadEntry(ppuVar8,0x100000));
LAB_0020b9d0:
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    return 0;
  case 10:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20ba20, guest::fatal(0x2ea490));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x34be) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    break;
  case 0xb:
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
  default:
    goto switchD_0020b624_default;
  }
  uVar7 = RT_CALLRA(0x20ba70, guest::sceDmaGetChan(1));
  RT_CALLRA(0x20ba80, guest::sceDmaSync(uVar7,0,0));
  RT_CALLRA(0x20ba88, guest::sendTexturePackets(0x100000));
switchD_0020b624_default:
  return 0;
}


RT_GUEST_FN(0x0020b5e8, guest::loadShopSet)

}  // namespace guest
