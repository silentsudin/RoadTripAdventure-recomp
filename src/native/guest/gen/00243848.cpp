// guest:0x00243848 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetNthStringByCategory(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
uint32_t pcVar2;
  
  pcVar2 = (uint32_t)0x0;
  switch(param_1) {
  case 1:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2eca74u + (uint32_t)(param_2 * 7) * 4)));
    break;
  case 2:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2eced4u + (uint32_t)(param_2 * 6) * 4)));
    break;
  case 3:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed1fcu + (uint32_t)(param_2 * 4) * 4)));
    break;
  case 4:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed32cu + (uint32_t)(param_2 * 7) * 4)));
    break;
  case 5:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed4ecu + (uint32_t)(param_2 * 4) * 4)));
    break;
  case 6:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2ed5d4u + (uint32_t)(param_2 * 0xb) * 4)));
    break;
  case 8:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2eff44u + (uint32_t)(param_2 * 4) * 4)));
    break;
  case 9:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2f0284u + (uint32_t)(param_2 * 3) * 4)));
    break;
  case 10:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2f02fcu + (uint32_t)(param_2 * 4) * 4)));
    break;
  case 0xb:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2f03e4u + (uint32_t)(param_2 * 5) * 4)));
    break;
  case 0xf:
    pcVar2 = (*(uint32_t*)rtguest::G((uint32_t)(0x2f0814u + (uint32_t)(param_2 * 2) * 4)));
  default:
  }
  param_3 = param_3 + -1;
  if (param_3 == -1) {
    return pcVar2;
  }
  do {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar2));
    while (pcVar2 = pcVar2 + 1, cVar1 == '\0') {
      param_3 = param_3 + -1;
      if (param_3 == -1) {
        return pcVar2;
      }
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)pcVar2));
    }
  } while( true );
}


RT_GUEST_FN(0x00243848, guest::GetNthStringByCategory)

}  // namespace guest
