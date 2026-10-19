// guest:0x002829e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define __sinit __sinit_decl
#include "guest_decls.inc"
#undef __sinit

namespace guest {

uint32_t __sinit(uint64_t param_1, ...) {
  int iVar1;
  
  iVar1 = (int)param_1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c))) = 0x2829a8;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38))) = 1;
  std(iVar1 + 0x1e4,4,0,param_1);
  std(iVar1 + 0x23c,9,1,param_1);
  std(iVar1 + 0x294,10,2,param_1);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1d8))) = 0;
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1e0))) = iVar1 + 0x1e4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1dc))) = 3;
  return 0;
}


RT_GUEST_FN(0x002829e8, guest::__sinit)

}  // namespace guest
