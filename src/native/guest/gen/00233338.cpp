// guest:0x00233338 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetShadowConfig(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
uint32_t puVar2;
  
  puVar2 = (uint32_t)(param_1 + 0x82e0);
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8430))) + 1;
  if (param_2 != 0) {
    if (iVar1 == 1) {
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0x1000000e0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fb2800000002;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x830c))) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8314))) = 0x1000000e0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x831c))) = 0x29fb4800000000;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8324))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x832c))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8334))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8334))) = 0x40000000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8330))) = 0x1e;
      return 0;
    }
    switch(iVar1 - (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x842c)))) {
    case 0:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fbc800000001;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    case 1:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fbe800000002;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    case 2:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fc0800000002;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    case 3:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fc2800000002;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    case -3:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0x1000000e0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fb2800000002;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0x1000000e0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fb6800000000;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
    case -2:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fb8800000000;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    case -1:
      (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0x14000000003;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82e8))) = 0xe0;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f0))) = 0x29fba800000000;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x82f8))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8300))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0;
      break;
    default:
      goto switchD_002334bc_default;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8308))) = 0x40000000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8304))) = 0x1e;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x830c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar2)) = 0;
switchD_002334bc_default:
}


RT_GUEST_FN(0x00233338, guest::SetShadowConfig)

}  // namespace guest
