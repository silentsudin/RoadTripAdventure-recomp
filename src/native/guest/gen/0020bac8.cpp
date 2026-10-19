// guest:0x0020bac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadFieldTile(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  char cVar1;
  ushort uVar2;
  int iVar3;
uint32_t puVar4;
uint32_t piVar5;
  int iVar6;
  int iVar7;
  undefined8 uVar8;
uint32_t piVar9;
uint32_t piVar10;
uint32_t puVar11;
  int iVar12;
  int iVar13;
uint32_t puVar14;
  int iVar15;
  
  iVar15 = (int)param_5;
  switch((*(uint32_t*)rtguest::G(0x3356c0))) {
  case 0:
    cdReadEntryPart(param_3,0x100000,0,0x800);
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    break;
  case 1:
    cdReadEntryPart(param_3,0x100800,0x800,(*(uint32_t*)rtguest::G(0x100004)) + -0x800);
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    break;
  case 2:
    puVar14 = (uint32_t)((*(uint32_t*)rtguest::G(0x100000)) + 0x100000);
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(uint32_t*)rtguest::G(0x100000))) * 1));
    puVar4 = puVar14;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
      }
      else {
        fatal(0x2ea490);
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x10))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x10)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar4 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar4 = puVar4 + (uint32_t)uVar2 * 8 + 8;
    }
    uVar8 = sceDmaGetChan(1);
    sceDmaSync(uVar8,0,0);
    sendTexturePackets(puVar14);
    iVar12 = 0x14;
    if ((*(uint32_t*)rtguest::G(0x100014)) != 0) {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
      iVar13 = 0x14;
      while( true ) {
        iVar12 = iVar13 + 4;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = cVar1 + '\x01';
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00100004 + iVar13))) == 0) break;
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
        iVar13 = iVar12;
      }
    }
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0xffffc))) - (*(uint32_t*)rtguest::G(0x100004));
    ::printf((char *)rtguest::G((uint32_t)(0x2ea5d0u)), (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))));
    ::printf((char *)rtguest::G((uint32_t)(0x2ea528u)), iVar12);
    if (param_5 == 3) {
      if (0x528000 < iVar12) {
LAB_0020bcdc:
        fatal(0x2ea5e0,iVar12);
      }
    }
    else if (0x3a8000 < iVar12) goto LAB_0020bcdc;
    cdReadEntryPart(param_3,0x707b80 + iVar15 * 0x3a8000,(*(uint32_t*)rtguest::G(0x100004)),iVar12);
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    break;
  case 3:
    iVar12 = iVar15 * 4;
    iVar13 = 0x40;
    piVar5 = (uint32_t)(0x707b80 + iVar15 * 0x3a8000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ab20 + (uint32_t)(iVar15) * 4)) = piVar5;
    piVar10 = piVar5;
    do {
      iVar13 = iVar13 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar10)) = (int)((*(int32_t*)rtguest::G((uint32_t)piVar10)) + (int)piVar5);
      piVar10 = piVar10 + 1;
    } while (-1 < iVar13);
    iVar6 = iVar15 * 0x3a8000 + ((*(uint32_t*)rtguest::G(0x100008)) - (*(uint32_t*)rtguest::G(0x100004)));
    piVar10 = (uint32_t)(0x707b80 + iVar6);
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar10));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0x70))) = piVar10;
    if (iVar13 == 0x600) {
      ::printf((char *)rtguest::G((uint32_t)(0x2ea5a0u)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0xe8))) = 0;
    }
    else if (iVar13 == 0x608) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0xe8))) =
           (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00708180 + iVar6))) + (int)piVar10);
    }
    else {
      fatal(0x2ea5b8);
    }
    iVar13 = 0xff;
    piVar10 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0x70)));
    piVar5 = piVar10;
    do {
      iVar13 = iVar13 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)piVar5)) + (int)piVar10;
      piVar5 = piVar5 + 1;
    } while (-1 < iVar13);
    iVar12 = (iVar15 * 0x3a8000 + (*(uint32_t*)rtguest::G(0x100010))) - (*(uint32_t*)rtguest::G(0x100004));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ab70 + (uint32_t)(iVar15) * 4)) = 0x707b80 + ((iVar15 * 0x3a8000 + (*(uint32_t*)rtguest::G(0x10000c))) - (*(uint32_t*)rtguest::G(0x100004)))
    ;
    piVar10 = (uint32_t)(0x707b80 + iVar12);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) != '\0') {
      if (param_4 == 0xb) {
        iVar13 = 0;
        puVar11 = 0x0177ab80;
        iVar12 = 0;
        do {
          (*(uint32_t*)rtguest::G((uint32_t)puVar11)) = piVar10;
          iVar6 = 0x40;
          piVar5 = piVar10;
          do {
            iVar6 = iVar6 + -1;
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)((*(int32_t*)rtguest::G((uint32_t)piVar5)) + (int)piVar10);
            piVar5 = piVar5 + 1;
          } while (-1 < iVar6);
          piVar5 = (uint32_t)((int)0x00100014 + iVar12);
          iVar13 = iVar13 + 1;
          piVar9 = (uint32_t)((int)0x00100010 + iVar12);
          iVar12 = iVar12 + 4;
          puVar11 = puVar11 + 1;
          piVar10 = (uint32_t)((int)piVar10 + ((*(int32_t*)rtguest::G((uint32_t)piVar5)) - (*(int32_t*)rtguest::G((uint32_t)piVar9))));
        } while (iVar13 < 0x34);
        piVar10 = (uint32_t)(param_2 + 0x80);
        iVar13 = 0;
        iVar12 = 0x10;
        do {
          iVar7 = iVar15 * 0x3a8000 + ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x1000d0))) - (*(uint32_t*)rtguest::G(0x100004)));
          piVar5 = (uint32_t)(0x707b80 + iVar7);
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
          (*(int32_t*)rtguest::G((uint32_t)piVar10)) = (int)piVar5;
          if (iVar6 == 0x600) {
            ::printf((char *)rtguest::G((uint32_t)(0x2ea5a0u)));
            (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(0x1e) * sizeof(int32_t))) = 0;
          }
          else if (iVar6 == 0x608) {
            (*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(0x1e) * sizeof(int32_t))) = (int)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00708180 + iVar7))) + (int)piVar5);
          }
          else {
            fatal(0x2ea5b8);
          }
          piVar5 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar10));
          iVar6 = 0xff;
          piVar9 = piVar5;
          do {
            iVar6 = iVar6 + -1;
            (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)piVar9)) + (int)piVar5;
            piVar9 = piVar9 + 1;
          } while (-1 < iVar6);
          iVar13 = iVar13 + 1;
          piVar10 = piVar10 + 1;
          iVar12 = iVar12 + 4;
        } while (iVar13 < 0x1a);
        piVar10 = (uint32_t)(0x707b80 +
                         ((iVar15 * 0x3a8000 + (*(uint32_t*)rtguest::G((uint32_t)0x10000c + (uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)))) * 4))) -
                         (*(uint32_t*)rtguest::G(0x100004))));
      }
      iVar12 = 0;
      if ((*(int32_t*)rtguest::G((uint32_t)piVar10 + (uint32_t)(2) * sizeof(int32_t))) == 0) {
        iVar13 = 0;
        piVar5 = piVar10;
      }
      else {
        iVar13 = 0;
        piVar5 = piVar10;
        do {
          if (iVar12 == 6) {
            fatal(0x2ea4a8);
            iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
          }
          else {
            iVar6 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
          }
          iVar7 = (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(3) * sizeof(int32_t)));
          iVar3 = iVar13 + iVar15 * 0x18;
          iVar13 = iVar13 + 4;
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f580 + iVar3))) = iVar6 + (int)piVar10;
          iVar12 = iVar12 + 1;
          piVar5 = piVar5 + 1;
        } while (iVar7 != 0);
        piVar5 = (uint32_t)(iVar13 + (int)piVar10);
      }
      puVar14 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar5)) + (int)piVar10);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar14 + 3)));
      puVar4 = puVar14;
      while (cVar1 != 'p') {
        if (cVar1 == '\x10') {
          uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
        }
        else {
          fatal(0x2ea490);
          uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar4));
        }
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x10))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x10)));
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar4 + (uint32_t)uVar2 * 0x10 + 0x13)));
        puVar4 = puVar4 + (uint32_t)uVar2 * 8 + 8;
      }
      uVar8 = sceDmaGetChan(1);
      sceDmaSync(uVar8,0,0);
      sendTexturePackets(puVar14);
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar10 + iVar13 + 4))) + (int)piVar10 != 0) {
        return 0;
      }
      return 1;
    }
    break;
  case 4:
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 * 4 + param_2 + 0x60))) = (int)param_4;
  }
  return 0;
}


RT_GUEST_FN(0x0020bac8, guest::loadFieldTile)

}  // namespace guest
