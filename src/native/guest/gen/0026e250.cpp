// guest:0x0026e250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsResetGraph(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  undefined8 uVar1;
uint32_t puVar2;
  
  if (param_1 == 1) {
    (*(uint8_t*)rtguest::G(0x12001000)) = 0x100;
  }
  else if (param_1 < 2) {
    if (param_1 == 0) {
      puVar2 = (uint32_t)sceGsGetEnv();
      (*(uint8_t*)rtguest::G(0x12001000)) = 0x200;
      (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = param_2;
      uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint16_t))) = param_3;
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint16_t))) = (uint32_t)((uint32_t)uVar1 >> 0x10) & 0xff;
      GsPutIMR(0xff00);
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint16_t))) = (uint32_t)(param_4 != 0);
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 4))) != 0) {
        disableIntc(2);
        RemoveIntcHandler(2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 6))));
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(6) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(7) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint16_t))) = 0;
      }
      SetGsCrt(param_2 & 1,param_3 & 0xff,param_4 & 1);
      return 0;
    }
  }
  else if (param_1 == 5) {
    puVar2 = (uint32_t)sceGsGetEnv();
    uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint16_t))) = (uint32_t)(param_4 != 0);
    (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = param_2;
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint16_t))) = param_3;
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint16_t))) = (uint32_t)((uint32_t)uVar1 >> 0x10) & 0xff;
    SetGsCrt(param_2 & 1,param_3 & 0xff,param_4 & 1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026e250, guest::sceGsResetGraph)

}  // namespace guest
