// guest:0x0024fcd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CarAiInit(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  ushort uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
uint32_t __s;
uint32_t puVar4;
  int iVar5;
  uint uVar6;
  long lVar7;
  long lVar8;
  int iVar9;
  
  __s = (uint32_t)((int)param_3 * 0x100 + 0x1759d70);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x25c))) = __s;
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x100));
  lVar7 = (long)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)));
  lVar8 = 0;
  if ((lVar7 == -1) || (lVar7 == 0x22)) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x80000) == 0) {
      if (param_3 != 0) {
        ::printf((char *)rtguest::G((uint32_t)(0x327ff0u)), param_4, param_3);
        lVar8 = carAiPathLookup(param_4,(int)param_3 + -1);
      }
    }
    else {
      ::printf((char *)rtguest::G((uint32_t)(0x327ff0u)), param_4, param_3);
      lVar8 = carAiPathLookup(param_4,param_3);
    }
    if (lVar8 != 0) {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)lVar8 + (uint32_t)(1) * 4));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)lVar8));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600))) = 0x002abf80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x250))) = uVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x254))) = uVar2;
    }
  }
  else {
    lVar8 = 6;
    if ((lVar7 != 0xb) && (lVar8 = 0xf, lVar7 != 0x10)) {
      lVar8 = lVar7;
    }
    iVar9 = (int)lVar8;
    puVar4 = (*(uint32_t*)rtguest::G((uint32_t)(0x2bd57cu + (uint32_t)(iVar9 * 2) * 4)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x250))) = (*(uint32_t*)rtguest::G((uint32_t)(0x2bd578u + (uint32_t)(iVar9 * 2) * 4)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x254))) = puVar4;
    if (lVar8 == 0x13) {
      puVar4 = 0x002abe80;
    }
    else if (lVar8 < 0x14) {
      uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x198)));
      if ((uVar1 & 0x30) != 0) {
        iVar5 = 0x3448;
        if ((uVar1 & 0x40) == 0) {
          iVar5 = 0;
        }
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600))) =
             iVar5 + (uVar1 & 0x20) * 0x80 + (char)(*(uint32_t*)rtguest::G((uint32_t)0x2ac080 + (uint32_t)(iVar9) * 4)) * 0x100 + 0x1780a60;
        return 0;
      }
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) == '\0') {
        uVar6 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x20)));
      }
      else {
        uVar6 = (uint32_t)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2ac098 + (uint32_t)((char)(*(uint32_t*)rtguest::G((uint32_t)0x002ac080u + (uint32_t)(iVar9) * 4))) * 4));
      }
      puVar4 = 0x002a9b80 + uVar6 * 0x100;
    }
    else {
      puVar4 = 0x002abf80;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600))) = puVar4;
  }
}


RT_GUEST_FN(0x0024fcd8, guest::CarAiInit)

}  // namespace guest
