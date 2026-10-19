// guest:0x00271e60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEmitGlyphsToGs(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  ulong uVar6;
  uint uVar7;
  int iVar8;
  
  param_6 = param_6 + -1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
  uVar7 = (*(int32_t*)rtguest::G((uint32_t)param_2)) + param_3 * iVar3;
  gifPkAppendTagNreg1(param_1,0,0,0);
  iVar8 = 0;
  if (param_6 != -1) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)param_5));
    iVar8 = 0;
    while ((uVar1 & 0xff) != 0) {
      param_5 = param_5 + 1;
      uVar6 = (uint32_t)uVar7;
      uVar7 = uVar7 + iVar3;
      iVar8 = iVar8 + 1;
      gifEmitBlock8Planes(param_1,uVar6 | (uint32_t)(uint32_t)(iVar5 + param_4 * iVar4) << 0x10 |
                                  0xffffffff00000000,iVar2,(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)((uVar1 >> 8 & 7) + 8) * sizeof(int32_t))),
                          (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x00330190 + (uVar1 & 0xff) * 8))),(*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(int32_t))));
      param_6 = param_6 + -1;
      if (param_6 == -1) {
      }
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)param_5));
    }
  }
}


RT_GUEST_FN(0x00271e60, guest::textEmitGlyphsToGs)

}  // namespace guest
