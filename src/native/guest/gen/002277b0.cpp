// guest:0x002277b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitSaveSlotDefaults(uint32_t param_1, ...) {
  uint uVar1;
  int iVar2;
  undefined1 uVar3;
uint32_t puVar4;
uint32_t puVar5;
uint32_t puVar6;
uint32_t puVar7;
  int iVar8;
uint32_t puVar9;
  int iVar10;
uint32_t __s;
  
  iVar2 = param_1 * 0x3448;
  __s = 0x177f760 + param_1 * 0x689;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x3448));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x1780a48 + iVar2))) = 0x40;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4a + iVar2))) = 0x80;
  iVar10 = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4c + iVar2))) = 0x20;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a4e + iVar2))) = 2;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a56 + iVar2))) = 0x10;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a58 + iVar2))) = 8;
  (*(uint16_t*)rtguest::G((uint32_t)0x1780a5a + (uint32_t)(param_1 * 0x1a24) * 2)) = 0x100;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a54 + iVar2))) = 8;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a50 + iVar2))) = 4;
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a52 + iVar2))) = 1;
  puVar9 = __s;
  puVar5 = 0x0177f768 + param_1 * 0xd12;
  do {
    puVar7 = puVar5 + 0x45c;
    puVar4 = (uint32_t)((int)__s + iVar10 + 0xff0);
    puVar6 = puVar9 + 0x208;
    iVar8 = 2;
    do {
      iVar8 = iVar8 + -1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar6)) = 0x7fffffff;
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 0x7fffffff;
      puVar7 = puVar7 + 0x1a;
      (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = 0xff;
      puVar4 = puVar4 + 0x1a;
      puVar6 = puVar6 + 0xd;
    } while (-1 < iVar8);
    iVar10 = iVar10 + 1;
    puVar9 = (uint32_t)((int)puVar9 + 4);
    puVar5 = puVar5 + 1;
  } while (iVar10 < 0x1a);
  iVar10 = 4;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780a10 + (uint32_t)(param_1 * 0xd12) * 4)) = 0x7fffffff;
  puVar5 = (uint32_t)(0x01780a24 + iVar2);
  do {
    iVar10 = iVar10 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0x7fffffff;
    puVar5 = puVar5 + -1;
  } while (-1 < iVar10);
  iVar10 = 9;
  puVar4 = 0x01780a39 + iVar2;
  do {
    iVar10 = iVar10 + -1;
    (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = 0x7f;
    puVar4 = puVar4 + -1;
  } while (-1 < iVar10);
  iVar8 = 7;
  iVar10 = 0xb70;
  do {
    iVar8 = iVar8 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)(0x0177f768 + param_1 * 0xd12) + iVar10))) = 0x7fffffff;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)__s + iVar10 + 0x12))) = 0xff;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f76c + iVar10 + iVar2))) = 0x7fffffff;
    iVar10 = iVar10 + 0xc;
  } while (-1 < iVar8);
  iVar10 = 0xa40;
  iVar8 = 9;
  do {
    puVar5 = (uint32_t)(iVar2 + 0x177f774 + iVar10);
    iVar8 = iVar8 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x0177f770 + iVar10 + iVar2))) = 0x7fffffff;
    iVar10 = iVar10 + 0x20;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = 0x7fffffff;
  } while (-1 < iVar8);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)0x177fdd0 + (uint32_t)(param_1 * 0xd12) * 4));
  (*(uint32_t*)rtguest::G((uint32_t)0x178033c + (uint32_t)(param_1 * 0xd12) * 4)) = 0x3dfff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780340 + iVar2))) = 0x7fbff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780384 + iVar2))) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)0x178034c + (uint32_t)(param_1 * 0xd12) * 4)) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780360 + iVar2))) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780364 + iVar2))) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780368 + iVar2))) = 0xf;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178037c + iVar2))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780380 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780388 + iVar2))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178038c + iVar2))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17803a4 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17803b8 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17803cc + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17803e0 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17803f4 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(int8_t*)rtguest::G((uint32_t)0x1780a3d + (uint32_t)(iVar2) * 1)) = 2;
  (*(int16_t*)rtguest::G((uint32_t)0x1780a3e + (uint32_t)(param_1 * 0x1a24) * 2)) = 0x5a;
  (*(uint64_t*)rtguest::G((uint32_t)0x177fca8 + (uint32_t)(param_1 * 0x689) * 8)) = 0xffffffffffffffff;
  (*(int64_t*)rtguest::G((uint32_t)0x177fcb0 + (uint32_t)(param_1 * 0x689) * 8)) = 0xfffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780344 + (uint32_t)(param_1 * 0xd12) * 4)) = 0x1fffff;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780350 + (uint32_t)(param_1 * 0xd12) * 4)) = 0xfffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780354 + iVar2))) = 0x3ffff;
  (*(uint32_t*)rtguest::G((uint32_t)0x178035c + (uint32_t)(param_1 * 0xd12) * 4)) = 0x007fffdf;
  (*(uint32_t*)rtguest::G((uint32_t)0x177fdd0 + (uint32_t)(param_1 * 0xd12) * 4)) = uVar1 | 0x100;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780348 + iVar2))) = 0xfbf;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780358 + iVar2))) = 0x17f;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0178036c + iVar2))) = 0x1f;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780370 + iVar2))) = 7;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780374 + iVar2))) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01780378 + iVar2))) = 3;
  (*(uint16_t*)rtguest::G((uint32_t)0x1780a3a + (uint32_t)(param_1 * 0x1a24) * 2)) = 0x3ff;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780408 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x17804a8 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x178041c + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780430 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780444 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780458 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x178046c + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780480 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1780494 + (uint32_t)(param_1 * 0xd12) * 4)) = 1;
  (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)((uint32_t)(0x0177fd8c + param_1 * 0xd12))), (char *)rtguest::G((uint32_t)(0x2eb470u)), 8));
  (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x0177fd94 + iVar2)), (char *)rtguest::G((uint32_t)(0x2eb478u)), 4));
  (*(uint32_t*)rtguest::G((uint32_t)0x177fd98 + (uint32_t)(iVar2) * 4)) = '\0';
  uVar3 = (*(uint32_t*)rtguest::G(0x3335fc));
  (*(uint32_t*)rtguest::G((uint32_t)0x177fd9f + (uint32_t)(param_1 * 0xd12) * 4)) = (*(uint32_t*)rtguest::G(0x3335f8));
  (*(uint32_t*)rtguest::G((uint32_t)0x177fda3 + (uint32_t)(iVar2) * 4)) = uVar3;
  CopyAiRouteTables(iVar2 + 0x1780a60);
  CopyAiRouteTables(iVar2 + 0x1781a60);
  return 0;
}


RT_GUEST_FN(0x002277b0, guest::InitSaveSlotDefaults)

}  // namespace guest
