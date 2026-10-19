// guest:0x00288720 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceFs_Rcv_Intr(uint32_t q0, ...) {
  int iVar1;
  undefined4 uVar2;
uint32_t puVar3;
uint32_t piVar4;
uint32_t puVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  size_t __n;
uint32_t puVar11;
  undefined8 uVar12;
  
  uVar2 = (*(uint32_t*)rtguest::G(0x21779544));
  iVar1 = (*(int32_t*)rtguest::G(0x21779540));
  if (-1 < (*(int32_t*)rtguest::G(0x21779540))) {
    (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x21779548)))), (char *)rtguest::G((uint32_t)(0x21779550)), (long)(*(uint32_t*)rtguest::G(0x2177954c))));
  }
  uVar10 = (*(uint32_t*)rtguest::G(0x21779570));
  uVar9 = (*(uint32_t*)rtguest::G(0x21779568));
  iVar8 = (*(uint32_t*)rtguest::G(0x2177955c));
  puVar3 = (*(uint32_t*)rtguest::G(0x21779554));
  switch(uVar2) {
  case 2:
    if ((0 < (int)(*(uint32_t*)rtguest::G(0x21779554))) && (0 < (int)(*(uint32_t*)rtguest::G(0x21779554)))) {
      puVar11 = 0x21779564;
      iVar6 = 0;
      do {
        iVar7 = iVar6 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar6))) = (*(uint8_t*)rtguest::G((uint32_t)puVar11));
        puVar11 = 0x21779565 + iVar6;
        iVar6 = iVar7;
      } while (iVar7 < (int)(*(uint32_t*)rtguest::G(0x21779554)));
    }
    iVar8 = (*(uint32_t*)rtguest::G(0x21779560));
    if ((0 < (*(uint32_t*)rtguest::G(0x21779558))) && (0 < (*(uint32_t*)rtguest::G(0x21779558)))) {
      puVar11 = 0x217795a4;
      iVar6 = 0;
      do {
        iVar7 = iVar6 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + iVar6))) = (*(uint8_t*)rtguest::G((uint32_t)puVar11));
        puVar11 = 0x217795a5 + iVar6;
        iVar6 = iVar7;
      } while (iVar7 < (*(uint32_t*)rtguest::G(0x21779558)));
    }
    goto LAB_00288a64;
  case 0xb:
    puVar3 = (uint32_t)0x21779558;
    puVar5 = (*(uint32_t*)rtguest::G(0x21779554));
    if (((uint32_t)(*(uint32_t*)rtguest::G(0x21779554)) & 7) == 0) {
      do {
        uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t))) = uVar10;
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t))) = uVar12;
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t))) = uVar9;
        puVar3 = puVar3 + 0x20;  // T* element step
        puVar5 = puVar5 + 0x20;  // T* element step
      } while (puVar3 != (uint32_t)0x21779698);
    }
    else {
      do {
        uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar12 = (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = (*(uint64_t*)rtguest::G((uint32_t)puVar3));
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t))) = uVar9;
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10;
        (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t))) = uVar12;
        puVar3 = puVar3 + 0x20;  // T* element step
        puVar5 = puVar5 + 0x20;  // T* element step
      } while (puVar3 != (uint32_t)0x21779698);
      puVar3 = (uint32_t)0x21779698;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar5)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar3));
    break;
  case 0xc:
    uVar12 = CONCAT44((*(uint32_t*)rtguest::G(0x21779564)),(*(uint32_t*)rtguest::G(0x21779560)));
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x21779554)))) = CONCAT44((*(uint32_t*)rtguest::G(0x2177955c)),(*(uint32_t*)rtguest::G(0x21779558)));
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(1) * sizeof(uint64_t))) = uVar12;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(2) * sizeof(uint64_t))) = uVar9;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(3) * sizeof(uint64_t))) = uVar10;
    uVar12 = (*(uint32_t*)rtguest::G(0x21779590));
    uVar10 = (*(uint32_t*)rtguest::G(0x21779588));
    uVar9 = (*(uint32_t*)rtguest::G(0x21779580));
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(4) * sizeof(uint64_t))) = (*(uint32_t*)rtguest::G(0x21779578));
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint64_t))) = uVar9;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(6) * sizeof(uint64_t))) = uVar10;
    (*(uint64_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(7) * sizeof(uint64_t))) = uVar12;
    break;
  case 0x17:
  case 0x19:
  case 0x1a:
    __n = (size_t)(*(uint32_t*)rtguest::G(0x21779558));
    if (0x400 < __n) {
      __n = 0x400;
    }
    (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x21779554)))), (char *)rtguest::G((uint32_t)(0x2177955c)), __n));
  }
LAB_00288a64:
  if (iVar1 < 0) {
    iVar1 = -iVar1;
    if ((*(uint32_t*)rtguest::G(0x2e9908)) == iVar1) {
      (*(uint32_t*)rtguest::G(0x2e9908)) = -1;
    }
    else {
      iVar8 = 1;
      do {
        if (0x1f < iVar8) {
          return 0;
        }
        piVar4 = 0x002e9908 + iVar8;
        iVar8 = iVar8 + 1;
      } while ((*(int32_t*)rtguest::G((uint32_t)piVar4)) != iVar1);
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = -1;
    }
  }
  else {
    iSignalSema();
  }
                    /* WARNING: Read-only address (ram,0x21779540) is written */
                    /* WARNING: Read-only address (ram,0x21779544) is written */
                    /* WARNING: Read-only address (ram,0x21779548) is written */
                    /* WARNING: Read-only address (ram,0x2177954c) is written */
                    /* WARNING: Read-only address (ram,0x21779554) is written */
                    /* WARNING: Read-only address (ram,0x21779558) is written */
                    /* WARNING: Read-only address (ram,0x2177955c) is written */
                    /* WARNING: Read-only address (ram,0x21779560) is written */
                    /* WARNING: Read-only address (ram,0x21779564) is written */
                    /* WARNING: Read-only address (ram,0x21779568) is written */
                    /* WARNING: Read-only address (ram,0x21779570) is written */
                    /* WARNING: Read-only address (ram,0x21779578) is written */
                    /* WARNING: Read-only address (ram,0x21779580) is written */
                    /* WARNING: Read-only address (ram,0x21779588) is written */
                    /* WARNING: Read-only address (ram,0x21779590) is written */
  return 0;
}


RT_GUEST_FN(0x00288720, guest::_sceFs_Rcv_Intr)

}  // namespace guest
