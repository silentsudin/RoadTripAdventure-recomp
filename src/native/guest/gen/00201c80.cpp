// guest:0x00201c80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiDrawTextRight(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  byte bVar1;
uint32_t pbVar2;
  int iVar3;
  uint uVar4;
uint32_t puVar5;
uint32_t pbVar6;
  int iVar7;
  
  iVar7 = 0;
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
  pbVar2 = param_3;
  do {
    uVar4 = (uint32_t)bVar1;
    if (uVar4 == 0) {
      if ((long)(*(uint32_t*)rtguest::G(0x335688)) != (param_4 & 0xffff)) {
        (*(uint32_t*)rtguest::G(0x335688)) = (int)(param_4 & 0xffff);
        if ((*(uint32_t*)rtguest::G(0x335684)) < 0xa00) {
          iVar3 = (*(uint32_t*)rtguest::G(0x335684)) * 0x10;
          (*(uint32_t*)rtguest::G(0x335684)) = (*(uint32_t*)rtguest::G(0x335684)) + 1;
          puVar5 = (uint32_t)((*(uint32_t*)rtguest::G(0x335680)) + iVar3 + 0x70);
          (*(uint64_t*)rtguest::G((uint32_t)puVar5)) = param_4 << 0x38 | 0x13780001400000;
          (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t))) = 0x16;
        }
      }
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
      iVar3 = param_1 - iVar7;
      while (bVar1 != 0) {
        iVar3 = uiDrawGlyph(iVar3,param_2 << 1,0,param_3);
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
        if (bVar1 == 0x2f) {
          param_3 = param_3 + 2;
        }
        else if (bVar1 == 0x60) {
          param_3 = param_3 + 2;
        }
        else if (bVar1 == 0x5e) {
          param_3 = param_3 + 2;
        }
        else if (bVar1 == 0x7e) {
          param_3 = param_3 + 2;
        }
        else if ((int)((uint32_t)bVar1 << 0x18) < 0) {
          param_3 = param_3 + 2;
        }
        else {
          param_3 = param_3 + 1;
        }
        bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_3));
      }
      return param_1 - iVar7;
    }
    pbVar6 = pbVar2 + 1;
    if (uVar4 == 0x7e) {
      uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar6));
LAB_00201cfc:
      pbVar6 = pbVar2 + 2;
LAB_00201d10:
      iVar7 = iVar7 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(uVar4 + 0x29a860)));
    }
    else {
      if (uVar4 == 0x5e) {
        uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar6));
        goto LAB_00201cfc;
      }
      if (uVar4 == 0x2f) {
        uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar6));
        goto LAB_00201cfc;
      }
      if (uVar4 == 0x60) {
        uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar6));
        goto LAB_00201cfc;
      }
      if (-1 < (int)(uVar4 << 0x18)) goto LAB_00201d10;
      iVar7 = iVar7 + 0x14;
      pbVar6 = pbVar2 + 2;
    }
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar6));
    pbVar2 = pbVar6;
  } while( true );
}


RT_GUEST_FN(0x00201c80, guest::uiDrawTextRight)

}  // namespace guest
