// guest:0x0020aef0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020aef0: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t loadCarPartsSet(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(80);
  char cVar1;
  ushort uVar2;
uint32_t __dest;
uint32_t puVar3;
  undefined8 uVar4;
  ulong uVar5;
  int iVar6;
  undefined8 uVar7;
  int iVar8;
uint32_t puVar9;
uint32_t puVar10;
  
  switch((*(uint32_t*)rtguest::G(0x3356c0))) {
  case 0:
    uVar7 = 0x100000;
    uVar4 = 0x29af64;
    break;
  case 1:
    puVar3 = 0x00100000;
    cVar1 = (*(uint8_t*)rtguest::G(0x100003));
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      else {
        RT_CALLRA(0x20af90, guest::fatal(0x2ea490));
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar3));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)uVar2 * 0x10 + 0x10;
    }
    uVar4 = RT_CALLRA(0x20afcc, guest::sceDmaGetChan(1));
    RT_CALLRA(0x20afdc, guest::sceDmaSync(uVar4,0,0));
    RT_CALLRA(0x20afe4, guest::sendTexturePackets(0x100000));
    return 0;
  case 2:
    uVar7 = 0x100000;
    uVar4 = 0x29af70;
    break;
  case 3:
    (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(0x0034a780)), (char *)rtguest::G((uint32_t)(0x00100000)), (long)(*(uint32_t*)rtguest::G(0x100030))));
    iVar6 = 0;
    iVar8 = 0xb;
    do {
      iVar8 = iVar8 + -1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eac8 + iVar6))) = 0x0034a780 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0034a780 + iVar6)));
      iVar6 = iVar6 + 4;
    } while (-1 < iVar8);
    puVar9 = (uint32_t)((*(uint32_t*)rtguest::G(0x100030)) + 0x100000);
    puVar10 = 0x0034a780 + (*(uint32_t*)rtguest::G(0x34a7b0));
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(uint32_t*)rtguest::G(0x100030))) * 1));
    puVar3 = puVar9;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20b0c0, guest::fatal(0x2ea490));
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar5 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar5 >> 0x20) & 0x3fff) + 0x1bc2) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    uVar4 = RT_CALLRA(0x20b110, guest::sceDmaGetChan(1));
    RT_CALLRA(0x20b120, guest::sceDmaSync(uVar4,0,0));
    RT_CALLRA(0x20b128, guest::sendTexturePackets(puVar9));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = puVar10;
    if (puVar10 != (uint32_t)0x0) {
      return 0;
    }
    return 1;
  case 4:
    uVar7 = 0x100000;
    uVar4 = 0x29af7c;
    break;
  case 5:
    __dest = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
    (uint32_t)(uintptr_t)(::memmove((char *)rtguest::G((uint32_t)(__dest)), (char *)rtguest::G((uint32_t)(0x00100000)), (long)(*(int32_t*)rtguest::G(0x100018))));
    iVar6 = 0;
    iVar8 = 5;
    do {
      iVar8 = iVar8 + -1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177eaf8 + iVar6))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + (int)__dest))) + (int)__dest;
      iVar6 = iVar6 + 4;
    } while (-1 < iVar8);
    puVar9 = (uint32_t)((*(int32_t*)rtguest::G(0x100018)) + 0x100000);
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)__dest + 0x18))) + (int)__dest;
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(int32_t*)rtguest::G(0x100018))) * 1));
    puVar3 = puVar9;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      else {
        RT_CALLRA(0x20b1e8, guest::fatal(0x2ea490));
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20)));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x20))) =
           uVar5 & 0xffffc000ffffffff |
           ((long)(int)(((uint32_t)(uVar5 >> 0x20) & 0x3fff) + 0x1c44) & 0x3fffU) << 0x20;
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x13)));
      puVar3 = puVar3 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar3)) * 0x10 + 0x10;
    }
    uVar4 = RT_CALLRA(0x20b238, guest::sceDmaGetChan(1));
    RT_CALLRA(0x20b248, guest::sceDmaSync(uVar4,0,0));
    RT_CALLRA(0x20b250, guest::sendTexturePackets(puVar9));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) = iVar6;
    if (iVar6 == 0) {
      return 1;
    }
    if (0x38a780 < iVar6) {
      RT_CALLRA(0x20b27c, guest::fatal(0x2ea4c0));
    }
    uVar4 = 0x29af88;
    uVar7 = 0x38a780;
    break;
  case 6:
    if (0x70000 < (*(uint32_t*)rtguest::G(0x38a7c0))) {
      RT_CALLRA(0x20b2d0, guest::fatal(0x2ea4d8));
    }
    iVar6 = 0;
    do {
      iVar8 = iVar6 + 1;
      (*(uint32_t*)rtguest::G((uint32_t)0x177ab30 + (uint32_t)(iVar6) * 4)) = 0x0038a780 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0038a780 + iVar6 * 4)));
      iVar6 = iVar8;
    } while (iVar8 < 0x10);
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
  default:
    goto switchD_0020af38_default;
  }
  RT_CALLRA(0x20b294, guest::cdReadEntry(uVar4,uVar7));
  (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
switchD_0020af38_default:
  return 0;
}


RT_GUEST_FN(0x0020aef0, guest::loadCarPartsSet)

}  // namespace guest
