// guest:0x0028c728 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceSifLoadModule(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint64_t param_5, ...) {
  undefined4 uVar1;
uint32_t puVar2;
  long lVar3;
uint32_t puVar4;
  undefined8 uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  
  lVar3 = _lf_bind();
  uVar1 = 0xffff0000;
  if (-1 < lVar3) {
    lVar3 = lfVersionCheck();
    if (lVar3 == 0) {
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)((uint32_t)0x01779e08)), (char *)rtguest::G((uint32_t)(param_1)), 0xfc));
      (*(uint32_t*)rtguest::G(0x1779f03)) = 0;
      if (param_3 == (uint32_t)0x0) {
        (*(uint32_t*)rtguest::G(0x1779f04)) = 0;
        (*(uint32_t*)rtguest::G(0x1779e00)) = 0;
      }
      else if ((long)param_2 < 0xfd) {
        (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)(0x01779f04)), (char *)rtguest::G((uint32_t)(param_3)), param_2));
        (*(uint32_t*)rtguest::G(0x1779e00)) = (uint32_t)param_2;
      }
      else {
        puVar4 = (uint32_t)0x01779f04;
        if ((((uint32_t)param_3 | 0x1779f04) & 7) == 0) {
          puVar2 = param_3 + (uint32_t)(0x1c) * sizeof(uint64_t);
          do {
            uVar5 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t)));
            uVar6 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t)));
            uVar7 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint64_t)));
            (*(uint64_t*)rtguest::G((uint32_t)puVar4)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint64_t))) = uVar7;
            param_3 = param_3 + (uint32_t)(4) * sizeof(uint64_t);
            puVar4 = puVar4 + (uint32_t)(4) * sizeof(uint64_t);
          } while (param_3 != puVar2);
        }
        else {
          puVar2 = param_3 + (uint32_t)(0x1c) * sizeof(uint64_t);
          do {
            uVar5 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t)));
            uVar6 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t)));
            uVar7 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint64_t)));
            (*(uint64_t*)rtguest::G((uint32_t)puVar4)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
            (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint64_t))) = uVar7;
            param_3 = param_3 + (uint32_t)(4) * sizeof(uint64_t);
            puVar4 = puVar4 + (uint32_t)(4) * sizeof(uint64_t);
          } while (param_3 != puVar2);
        }
        uVar5 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar6 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + (uint32_t)(3) * sizeof(uint64_t))));
        (*(uint64_t*)rtguest::G((uint32_t)puVar4)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
        (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t))) = uVar5;
        (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t))) = uVar6;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + (uint32_t)(3) * sizeof(uint64_t)))) = uVar1;
        (*(uint32_t*)rtguest::G(0x1779e00)) = 0xfc;
      }
      lVar3 = sceSifCallRpc(0x177a000,param_5,0,0x1779e00,0x200,0x1779e00,8,0);
      uVar1 = (*(uint32_t*)rtguest::G(0x1779e00));
      if (lVar3 < 0) {
        uVar1 = 0xfffeffff;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)param_4)) = (*(uint32_t*)rtguest::G(0x1779e04));
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028c728, guest::_sceSifLoadModule)

}  // namespace guest
