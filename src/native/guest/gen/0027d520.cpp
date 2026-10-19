// guest:0x0027d520 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __frame_state_for(uint32_t param_1, uint32_t param_2, ...) {
uint32_t apcStack_4f0 = rtguest::allocFrame(32);
uint32_t auStack_4d0 = rtguest::allocFrame(1120);
uint32_t auStack_70 = rtguest::allocFrame(32);
    uint32_t iStack_50 = rtguest::allocFrame(4);
uint32_t auStack_4c = rtguest::allocFrame(12);
uint32_t puVar1;
  uint uVar2;
  int iVar3;
uint32_t puVar4;
uint32_t puVar5;
  long lVar6;
uint32_t piVar7;
uint32_t puVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
uint32_t piVar12;
  
  lVar6 = find_fde(param_1);  // 27d544: $4 intact since 27d52c entry; pc lookup
  puVar5 = (uint32_t)0x0;
  if (lVar6 != 0) {
    uVar2 = extract_cie_info(lVar6,apcStack_4f0);
    puVar5 = (uint32_t)0x0;
    if (uVar2 != 0) {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(auStack_4d0)), 0, 0x480));
      piVar12 = (uint32_t)lVar6;
      piVar7 = (uint32_t)((int)piVar12 + (4 - (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(1) * sizeof(int32_t)))));
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      for (; uVar2 < (uint32_t)((int)piVar7 + iVar3 + 4);
          uVar2 = execute_cfa_insn(uVar2,auStack_4d0,apcStack_4f0,0)) {
      }
      piVar7 = piVar12 + 4;
      if ((*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apcStack_4f0 + (uint32_t)(0) * 4)))) == 'z') {
        iVar3 = decode_uleb128(piVar7,iStack_50);
        piVar7 = (uint32_t)(iVar3 + (*(int32_t*)rtguest::G((uint32_t)iStack_50)));
      }
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar12));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) = (*(int32_t*)rtguest::G((uint32_t)piVar12 + (uint32_t)(2) * sizeof(int32_t)));
      while ((piVar7 < (uint32_t)((int)piVar12 + iVar3 + 4) && ((*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) <= param_1))) {
        piVar7 = (uint32_t)execute_cfa_insn(piVar7,auStack_4d0,apcStack_4f0,auStack_4c);
      }
      puVar5 = auStack_4d0;
      puVar1 = param_2;
      do {
        puVar4 = puVar1;
        puVar8 = puVar5;
        uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint64_t)));
        uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(2) * sizeof(uint64_t)));
        uVar11 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(3) * sizeof(uint64_t)));
        (*(uint64_t*)rtguest::G((uint32_t)puVar4)) = (*(uint64_t*)rtguest::G((uint32_t)puVar8));
        (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t))) = uVar9;
        (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t))) = uVar10;
        (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint64_t))) = uVar11;
        puVar5 = puVar8 + 4;
        puVar1 = puVar4 + 4;
      } while (puVar5 != auStack_70);
      uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(5) * sizeof(uint64_t)));
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(6) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(4) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)puVar5));
      (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(5) * sizeof(uint64_t))) = uVar9;
      (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(6) * sizeof(uint64_t))) = uVar10;
      puVar5 = param_2;
    }
  }
  return puVar5;
}


RT_GUEST_FN(0x0027d520, guest::__frame_state_for)

}  // namespace guest
