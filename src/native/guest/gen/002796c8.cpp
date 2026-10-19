// guest:0x002796c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sdrCB(uint32_t param_1, uint32_t param_2, ...) {
uint32_t pcVar1;
  
  switch((*(uint32_t*)rtguest::G((uint32_t)param_2))) {
  case 1:
    pcVar1 = (*(uint32_t*)rtguest::G(0x2e8984));
    goto LAB_0027971c;
  case 2:
    pcVar1 = (*(uint32_t*)rtguest::G(0x2e8988));
  case 3:
    pcVar1 = (*(uint32_t*)rtguest::G(0x2e898c));
LAB_0027971c:
    if (pcVar1 == (uint32_t)0x0) {
      return 0;
    }
    rtCall(pcVar1);
  case 0xb:
    if ((*(uint32_t*)rtguest::G(0x2e8990)) != (uint32_t)0x0) {
      rtCall((*(uint32_t*)rtguest::G(0x2e8990)), 0, (*(uint32_t*)rtguest::G(0x2e899c)));
    }
    break;
  case 0xc:
    if ((*(uint32_t*)rtguest::G(0x2e8994)) != (uint32_t)0x0) {
      rtCall((*(uint32_t*)rtguest::G(0x2e8994)), 1, (*(uint32_t*)rtguest::G(0x2e89a0)));
    }
    break;
  case 0xd:
    if ((*(uint32_t*)rtguest::G(0x2e8998)) != (uint32_t)0x0) {
      rtCall((*(uint32_t*)rtguest::G(0x2e8998)), (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t))), (*(uint32_t*)rtguest::G(0x2e89a4)));
    }
  }
}


RT_GUEST_FN(0x002796c8, guest::_sdrCB)

}  // namespace guest
