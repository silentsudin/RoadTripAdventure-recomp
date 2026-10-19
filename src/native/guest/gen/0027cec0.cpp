// guest:0x0027cec0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t extract_cie_info(uint32_t param_1, uint32_t param_2, ...) {
  undefined4 uVar1;
  int iVar2;
uint32_t pbVar3;
  size_t sVar4;
  undefined8 uVar5;
uint32_t pcVar6;
uint32_t pcVar7;
uint32_t aiStack_30 = rtguest::allocFrame(16);
  
  pcVar6 = (uint32_t)((param_1 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4)))) + 0xd);
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = pcVar6;
  iVar2 = ::strcmp((char *)rtguest::G((uint32_t)(pcVar6)), (char *)rtguest::G((uint32_t)(0x1ff007u)));
  pcVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
  if (iVar2 != 0) {
    iVar2 = ::strcmp((char *)rtguest::G((uint32_t)(pcVar6)), (char *)rtguest::G((uint32_t)(0x21cbe6u)));
    if (iVar2 == 0) {
      pcVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
    }
    else {
      pcVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
      if ((*(int8_t*)rtguest::G((uint32_t)pcVar6)) != 'z') {
        return (uint32_t)0x0;
      }
    }
  }
  sVar4 = ::strlen((char *)rtguest::G((uint32_t)(pcVar6)));
  pcVar6 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
  pcVar7 = pcVar6 + (int)sVar4 + 1;
  iVar2 = ::strcmp((char *)rtguest::G((uint32_t)(pcVar6)), (char *)rtguest::G((uint32_t)(0x21cbe6u)));
  if (iVar2 == 0) {
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)pcVar7));
    pcVar7 = pcVar6 + (int)sVar4 + 5;
    (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = uVar1;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
  }
  uVar5 = decode_uleb128(pcVar7,param_2 + 2);
  pbVar3 = (uint32_t)decode_sleb128(uVar5,param_2 + 3);
  (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint32_t))) = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar3));
  pbVar3 = pbVar3 + 1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2)))) == 'z') {
    iVar2 = decode_uleb128(pbVar3,aiStack_30);
    pbVar3 = (uint32_t)(iVar2 + (*(int32_t*)rtguest::G((uint32_t)aiStack_30 + (uint32_t)(0) * 4)));
  }
  return pbVar3;
}


RT_GUEST_FN(0x0027cec0, guest::extract_cie_info)

}  // namespace guest
