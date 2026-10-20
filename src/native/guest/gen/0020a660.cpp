// guest:0x0020a660 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMenuInputState(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  char cVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  char cVar5;
uint32_t pbVar6;
uint32_t pcVar7;
uint32_t pbVar8;
uint32_t piVar9;
uint32_t pbVar10;
uint32_t puVar11;
uint32_t piVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  
  iVar13 = 0;
  iVar14 = 0;
  piVar12 = 0x003356b8;
  iVar15 = 0;
  do {
    if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x00333358 + iVar15))) != 0) &&
       ((cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00335698 + iVar13))), cVar5 = cVar1 + -1, cVar1 == '\0' ||
        ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00335698 + iVar13))) = cVar5, cVar5 == '\0')))) {
      piVar9 = (uint32_t)(0x00333358 + iVar15);
      pbVar10 = (uint32_t)(iVar14 + 0x3356a9);
      puVar11 = 0x003356a8 + iVar14;
      pbVar8 = 0x003356b0 + iVar13;
      do {
        bVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9))));
        pbVar6 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9)) + 1;
        if (bVar2 == 0x82) {
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
          (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (int)pbVar6;
          bVar3 = (*(int8_t*)rtguest::G((uint32_t)pbVar6));
          (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 | 2;
          (*(int8_t*)rtguest::G((uint32_t)pbVar10)) = bVar3;
        }
        else if (bVar2 < 0x83) {
          if (bVar2 == 0x80) {
            bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
            (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = 1;
            (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 | 1;
          }
          else if (bVar2 == 0x81) {
            bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
            (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = 0;
            (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 & 0xfe;
          }
          else {
            pcVar7 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9));
LAB_0020a7e0:
            (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00335698 + iVar13))) = (*(int8_t*)rtguest::G((uint32_t)pcVar7));
          }
        }
        else if (bVar2 == 0x84) {
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
          (*(int8_t*)rtguest::G((uint32_t)pbVar10)) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(0x003356a0 + iVar13)));
          (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 | 2;
        }
        else {
          if (0x83 < bVar2) {
            if (bVar2 != 0xff) {
              pcVar7 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar9));
              goto LAB_0020a7e0;
            }
            bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
            (*(uint8_t*)rtguest::G((uint32_t)puVar11)) = 0;
            (*(int32_t*)rtguest::G((uint32_t)piVar9)) = 0;
            (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 & 0xfc;
            (*(int8_t*)rtguest::G((uint32_t)pbVar10)) = 0;
            break;
          }
          bVar2 = (*(int8_t*)rtguest::G((uint32_t)pbVar8));
          (*(int8_t*)rtguest::G((uint32_t)pbVar10)) = 0;
          (*(int8_t*)rtguest::G((uint32_t)pbVar8)) = bVar2 & 0xfd;
        }
        cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x00335698 + iVar13)));
        (*(int32_t*)rtguest::G((uint32_t)piVar9)) = (*(int32_t*)rtguest::G((uint32_t)piVar9)) + 1;
      } while (cVar1 == '\0');
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(0x003356b0 + iVar13))) == '\0') {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      if (0xb3 < iVar4) goto LAB_0020a830;
      (*(int32_t*)rtguest::G((uint32_t)piVar12)) = 0;
    }
    else {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
LAB_0020a830:
      (*(int32_t*)rtguest::G((uint32_t)piVar12)) = iVar4 + 1;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)piVar12)) == 0x168) {
      (*(int32_t*)rtguest::G((uint32_t)piVar12)) = 0;
    }
    else if (0xb3 < (*(int32_t*)rtguest::G((uint32_t)piVar12))) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar14 + 0x3356a9))) = 0;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(0x003356a8 + iVar14))) = 0;
    }
    iVar13 = iVar13 + 1;
    iVar14 = iVar14 + 2;
    piVar12 = piVar12 + 4;
    iVar15 = iVar15 + 4;
    if (1 < iVar13) {
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x0020a660, guest::UpdateMenuInputState)

}  // namespace guest
