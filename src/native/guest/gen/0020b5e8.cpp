// guest:0x0020b5e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadShopSet(uint32_t param_1, uint32_t param_2, ...) {
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
    cdReadEntryPart(0x29b29c,0x100000,0,0x3f000);
    goto LAB_0020b9d0;
  case 1:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1f2e) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
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
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x1cc6) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
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
        fatal(0x2ea490);
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)uVar2 * 8 + 8;
    }
    break;
  case 6:
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) & 1) == 0) {
      ppuVar8 = 0x29afe8u;
    }
    else {
      ppuVar8 = 0x29b048u;
    }
    lVar5 = cdReadEntry(ppuVar8,0xaafb80);
    ::printf((char *)rtguest::G((uint32_t)(0x2ea528u)), lVar5);
    if (0xaf8000 < lVar5) {
      fatal(0x2ea540,lVar5,0xaf800);
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
      piVar4 = piVar4 + 1;
    } while (-1 < iVar9);
    ppuVar8 = 0x29b054u;
    goto LAB_0020b9c4;
  case 8:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x285e) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
    }
    break;
  case 9:
    ppuVar8 = 0x29b150u;
LAB_0020b9c4:
    cdReadEntry(ppuVar8,0x100000);
LAB_0020b9d0:
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    return 0;
  case 10:
    puVar3 = 0x00100000;
    cVar1 = (*(int8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      else {
        fatal(0x2ea490);
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x10))) =
           uVar6 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar6 >> 0x20) & 0x3fff) + 0x34be) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 8 + 8;
    }
    break;
  case 0xb:
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
  default:
    goto switchD_0020b624_default;
  }
  uVar7 = sceDmaGetChan(1);
  sceDmaSync(uVar7,0,0);
  sendTexturePackets(0x100000);
switchD_0020b624_default:
  return 0;
}


RT_GUEST_FN(0x0020b5e8, guest::loadShopSet)

}  // namespace guest
