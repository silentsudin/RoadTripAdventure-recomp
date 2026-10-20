// guest:0x0020c188 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020c188: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t loadCourseTile(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(112);
    uint32_t iStack_68 = rtguest::allocFrame(4);
  char cVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
uint32_t puVar5;
uint32_t piVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  short sVar10;
uint32_t piVar11;
  int iVar12;
uint32_t puVar13;
  int iVar14;
  
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
    puVar13 = (uint32_t)((*(uint32_t*)rtguest::G(0x100000)) + 0x100000);
    cVar1 = (*(uint8_t*)rtguest::G((uint32_t)0x100003 + (uint32_t)((*(uint32_t*)rtguest::G(0x100000))) * 1));
    puVar5 = puVar13;
    while (cVar1 != 'p') {
      if (cVar1 == '\x10') {
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
      }
      else {
        fatal(0x2ea490);
        uVar2 = (*(uint16_t*)rtguest::G((uint32_t)puVar5));
      }
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x20))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x20)));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + (uint32_t)uVar2 * 0x10 + 0x13)));
      puVar5 = puVar5 + (uint32_t)uVar2 * 0x10 + 0x10;
    }
    uVar9 = sceDmaGetChan(1);
    sceDmaSync(uVar9,0,0);
    sendTexturePackets(puVar13);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
    if (cVar1 < '\x12') {
      iVar12 = 5;
    }
    else if (((((cVar1 == '\x13') || (cVar1 == '\x14')) || (cVar1 == '\x16')) ||
             (((cVar1 == '\x17' || (cVar1 == '\x19')) ||
              ((cVar1 == '\x1a' || ((cVar1 == '\x1c' || (cVar1 == '%')))))))) ||
            (iVar12 = 4, cVar1 == '&')) {
      iVar12 = 5;
    }
    iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 * 4 + 0x100000)));
    iVar12 = iVar12 * 4;
    while (iVar14 != 0) {
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) + '\x01';
      iVar14 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00100004 + iVar12)));
      iVar12 = iVar12 + 4;
    }
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0xffffc))) - (*(uint32_t*)rtguest::G(0x100004));
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea528u)), iVar12);
    if (0x750000 < iVar12) {
      fatal(0x2ea5e0,iVar12);
    }
    cdReadEntryPart(param_3,0x707b80 + param_5 * 0x3a8000,(*(uint32_t*)rtguest::G(0x100004)),iVar12);
    (*(uint32_t*)rtguest::G(0x3356c0)) = (*(uint32_t*)rtguest::G(0x3356c0)) | 0x8000;
    break;
  case 3:
    iVar12 = param_5 * 4;
    iVar14 = 0x40;
    piVar6 = (uint32_t)(0x707b80 + param_5 * 0x3a8000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ab20 + (uint32_t)(param_5) * 4)) = piVar6;
    piVar11 = piVar6;
    do {
      iVar14 = iVar14 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar11)) = (int)((*(int32_t*)rtguest::G((uint32_t)piVar11)) + (int)piVar6);
      piVar11 = piVar11 + sizeof(int32_t);
    } while (-1 < iVar14);
    iVar7 = param_5 * 0x3a8000 + ((*(uint32_t*)rtguest::G(0x100008)) - (*(uint32_t*)rtguest::G(0x100004)));
    piVar11 = (uint32_t)(0x707b80 + iVar7);
    iVar14 = (*(int32_t*)rtguest::G((uint32_t)piVar11));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0x70))) = piVar11;
    if (iVar14 == 0x600) {
      rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ea5a0u)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0xe8))) = 0;
    }
    else if (iVar14 == 0x608) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0xe8))) =
           (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00708180 + iVar7))) + (int)piVar11);
    }
    else {
      fatal(0x2ea5b8);
    }
    iVar14 = 0xff;
    piVar11 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + param_2 + 0x70)));
    piVar6 = piVar11;
    do {
      iVar14 = iVar14 + -1;
      (*(int32_t*)rtguest::G((uint32_t)piVar6)) = (*(int32_t*)rtguest::G((uint32_t)piVar6)) + (int)piVar11;
      iVar7 = (*(uint32_t*)rtguest::G(0x100010));
      iVar12 = (*(uint32_t*)rtguest::G(0x100004));
      piVar6 = piVar6 + sizeof(int32_t);
    } while (-1 < iVar14);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
    piVar11 = (uint32_t)(0x707b80 + ((param_5 * 0x3a8000 + (*(uint32_t*)rtguest::G(0x10000c))) - (*(uint32_t*)rtguest::G(0x100004))));
    if (((((((cVar1 < '\x12') || (cVar1 == '\x13')) || (cVar1 == '\x14')) ||
          ((cVar1 == '\x16' || (cVar1 == '\x17')))) ||
         ((cVar1 == '\x19' || ((cVar1 == '\x1a' || (cVar1 == '\x1c')))))) || (cVar1 == '%')) ||
       (cVar1 == '&')) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177ab70 + (uint32_t)(param_5) * 4)) = piVar11;
      piVar11 = (uint32_t)(0x707b80 + ((param_5 * 0x3a8000 + iVar7) - iVar12));
    }
    iVar12 = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) != '\0') {
      (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = 0;
      sVar10 = 0x2b26;
      while( true ) {
        iVar14 = 0;
        if ((*(int32_t*)rtguest::G((uint32_t)piVar11 + (uint32_t)(2) * sizeof(int32_t))) == 0) {
          iVar7 = 0;
          piVar6 = piVar11;
        }
        else {
          iVar7 = 0;
          piVar6 = piVar11;
          do {
            if (iVar14 == 6) {
              fatal(0x2ea4a8);
              iVar8 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
            }
            else {
              iVar8 = (*(int32_t*)rtguest::G((uint32_t)piVar6));
            }
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar6 + (uint32_t)(3) * sizeof(int32_t)));
            iVar4 = iVar7 + (*(int32_t*)rtguest::G((uint32_t)iStack_68));
            iVar7 = iVar7 + 4;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x177f580 + iVar4))) = (uint32_t)(iVar8 + (int)piVar11);
            iVar14 = iVar14 + 1;
            piVar6 = piVar6 + sizeof(int32_t);
          } while (iVar3 != 0);
          piVar6 = (uint32_t)(iVar7 + (int)piVar11);
        }
        puVar13 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar6)) + (int)piVar11);
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar13 + 3)));
        puVar5 = puVar13;
        while (cVar1 != 'p') {
          if (cVar1 != '\x10') {
            fatal(0x2ea490);
          }
          (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x20))) =
               (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x20))) & 0xffffc000ffffffff |
               ((long)(int)(((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar5 + 0x20))) >> 0x20) & 0x3fff) + (int)sVar10) &
               0x3fffU) << 0x20;
          cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar5 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar5)) * 0x10 + 0x13)));
          puVar5 = puVar5 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar5)) * 0x10 + 0x10;
        }
        uVar9 = sceDmaGetChan(1);
        sceDmaSync(uVar9,0,0);
        sendTexturePackets(puVar13);
        piVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar11 + iVar7 + 4))) + (int)piVar11);
        if (piVar11 == (uint32_t)0x0) break;
        iVar12 = iVar12 + 1;
        sVar10 = sVar10 + 0x44;
        (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 0x18;
        if ((int)(uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) <= iVar12) {
          return 0;
        }
      }
      return 1;
    }
    break;
  case 4:
    (*(uint32_t*)rtguest::G(0x3356c0)) = 0xffffffff;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_5 * 4 + param_2 + 0x60))) = param_4;
  }
  return 0;
}


RT_GUEST_FN(0x0020c188, guest::loadCourseTile)

}  // namespace guest
