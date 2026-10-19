// guest:0x00270c18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridInitAttrs(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xf) * sizeof(uint32_t))) = 0x80ffffff;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = 0x10;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = 0x80;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint32_t))) = 0x3c;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint32_t))) = 0x80000000;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint32_t))) = 0x80ff0000;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint32_t))) = 0x800000ff;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint32_t))) = 0x80ff00ff;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint32_t))) = 0x8000ff00;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xd) * sizeof(uint32_t))) = 0x80ffff00;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0x8000ffff;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = 0x80;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint32_t))) = 0;
  return 0;
}


RT_GUEST_FN(0x00270c18, guest::textGridInitAttrs)

}  // namespace guest
