// guest:0x00283788 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _multadd(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  int iVar1;
  undefined8 uVar2;
  uint uVar3;
  uint uVar4;
uint32_t puVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_2;
  puVar5 = (uint32_t)(iVar7 + 0x14);
  iVar6 = 0;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10)));
  do {
    iVar6 = iVar6 + 1;
    uVar4 = ((*(uint32_t*)rtguest::G((uint32_t)puVar5)) & 0xffff) * param_3 + param_4;
    uVar3 = ((*(uint32_t*)rtguest::G((uint32_t)puVar5)) >> 0x10) * param_3 + (uVar4 >> 0x10);
    param_4 = uVar3 >> 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)puVar5)) = uVar3 * 0x10000 + (uVar4 & 0xffff);
    puVar5 = puVar5 + 0x4;  // T* element step
  } while (iVar6 < iVar1);
  if (param_4 != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) <= iVar1) {
      uVar2 = _Balloc(param_1,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) + 1);
      (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)((uint32_t)((int)uVar2 + 0xc))), (char *)rtguest::G((uint32_t)((uint32_t)(iVar7 + 0xc))), (long)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x10))) * 4 + 8)));
      _Bfree(param_1,param_2);
      param_2 = uVar2;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 * 4 + (int)param_2 + 0x14))) = param_4;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x10))) = iVar1 + 1;
  }
  return param_2;
}


RT_GUEST_FN(0x00283788, guest::_multadd)

}  // namespace guest
