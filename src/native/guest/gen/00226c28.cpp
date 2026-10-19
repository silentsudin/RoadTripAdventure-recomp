// guest:0x00226c28 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifBatchRegs(uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, ...) {
uint32_t puVar1;
  bool bVar2;
  ulong uVar3;
  
  if ((*(uint32_t*)rtguest::G(0x3356ec)) == (uint32_t)0x0) {
    fatal(0x2eafa0);
  }
  if ((*(uint32_t*)rtguest::G(0x3356f4)) == 0) {
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(0xc) * sizeof(undefined4) < (uint32_t)0x17523d0) {
      bVar2 = false;
    }
    else {
      fatal(0x2eafc0);
      bVar2 = false;
    }
  }
  else {
    bVar2 = true;
    if ((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(0xc) * sizeof(undefined4) < (uint32_t)0x1750bd0) {
      bVar2 = false;
    }
  }
  if (!bVar2) {
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)))) = 0x10000002;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = 0x6c028192;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x01000101;
    (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(4) * sizeof(undefined4)))) = (*(uint64_t*)rtguest::G(0x29ed10));
    uVar3 = (*(uint64_t*)rtguest::G(0x29ed18));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(8) * sizeof(undefined4)))) = param_1;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(10) * sizeof(undefined4)))) = param_3;
    puVar1 = (uint32_t)((*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(6) * sizeof(undefined4));
    ( *(uint32_t*)rtguest::G(0x3356ec)) = (*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(0xc) * sizeof(undefined4);
    (*(uint64_t*)rtguest::G((uint32_t)puVar1)) = uVar3 & 0xffffffffffffff00 | param_2 & 0xf | (param_4 & 0xf) << 4;
  }
  return 0;
}


RT_GUEST_FN(0x00226c28, guest::vifBatchRegs)

}  // namespace guest
