// guest:0x00227178 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vifEndPacket(uint32_t param_1, ...) {
  int iVar1;
uint32_t puVar2;
  
  if ((*(uint32_t*)rtguest::G(0x3356f4)) != param_1) {
    fatal(0x2eb008);
  }
  iVar1 = (*(uint32_t*)rtguest::G(0x3356f4));
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(0) * 4)))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(1) * 4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(3) * 4)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(0) * 4)))) = 0x70000000;
  (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G(0x3356ec)) + (uint32_t)(2) * 4)) = 0x6000000;
  if (iVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x3356ec)) = (uint32_t)0x0;
    puVar2 = 0x01750bf0;
  }
  else {
    (*(uint32_t*)rtguest::G(0x3356f4)) = 0;
    puVar2 = 0x01730bf0;
    (*(uint32_t*)rtguest::G(0x3356ec)) = (uint32_t)(*(uint32_t*)rtguest::G(0x3356f0));
    (*(uint32_t*)rtguest::G(0x3356f0)) = 0;
  }
  return puVar2;
}


RT_GUEST_FN(0x00227178, guest::vifEndPacket)

}  // namespace guest
