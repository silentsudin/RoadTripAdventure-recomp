// guest:0x00238950 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AwardRaceResult(uint32_t param_1, ...) {
  char cVar1;
  undefined4 uVar2;
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  if (cVar1 == '&') {
    if ((*(int8_t*)rtguest::G(0x177ae97)) == '\0') {
      uVar2 = 2;
    }
    else {
      uVar2 = 1;
    }
    GetOrSetPresidentRaceResult(uVar2);
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  }
  if (cVar1 == '(') {
    if (((*(uint32_t*)rtguest::G(0x177ace4)) < 280.0) && ((*(uint32_t*)rtguest::G(0x177af54)) < 280.0)) {
      GetOrSetPresidentRaceResult(3);
    }
    if ((280.0 <= (*(uint32_t*)rtguest::G(0x177ace4))) && (280.0 <= (*(uint32_t*)rtguest::G(0x177af54)))) {
      if ((*(int8_t*)rtguest::G(0x177ae97)) != '\0') {
        GetOrSetPresidentRaceResult(1);
        return 0;
      }
      GetOrSetPresidentRaceResult(2);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00238950, guest::AwardRaceResult)

}  // namespace guest
