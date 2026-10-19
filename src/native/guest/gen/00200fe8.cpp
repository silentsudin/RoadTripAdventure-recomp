// guest:0x00200fe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiKanaGlyphUV(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  short sVar1;
  
  (*(int16_t*)rtguest::G((uint32_t)param_2)) = param_4;
  switch(param_3) {
  case 0x8141:
    sVar1 = 0x1a4;
    break;
  case 0x8142:
    sVar1 = 0x1b8;
    break;
  default:
    if (param_3 < 0x8754) {
      if (0x833f < param_3) {
        (*(int16_t*)rtguest::G((uint32_t)param_1)) = ((uint16_t)(param_3 + 0x7cc8) % 0x19) * 0x14;
        (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + ((uint16_t)(param_3 + 0x7cc8) / 0x19) * 0x14 + 0x3c;
        return 0;
      }
      if (0x829e < param_3) {
        (*(int16_t*)rtguest::G((uint32_t)param_1)) = ((uint16_t)(param_3 + 0x7d61) % 0x19) * 0x14;
        (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + ((uint16_t)(param_3 + 0x7d61) / 0x19) * 0x14;
        return 0;
      }
      if (0x825f < param_3) {
        (*(int16_t*)rtguest::G((uint32_t)param_1)) = ((uint16_t)(param_3 + 0x7da4) % 0x19) * 0x14;
        (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + ((uint16_t)(param_3 + 0x7da4) / 0x19) * 0x14 + 0x8c;
        return 0;
      }
      (*(int16_t*)rtguest::G((uint32_t)param_1)) = ((uint16_t)(param_3 + 0x7dc4) % 0x19) * 0x14;
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)param_2)) + ((uint16_t)(param_3 + 0x7dc4) / 0x19) * 0x14;
      goto LAB_002012e8;
    }
    sVar1 = (param_3 + 0x78ac) * 0x14 + 0x78;
    goto LAB_00201158;
  case 0x8145:
    sVar1 = 400;
    break;
  case 0x8148:
    sVar1 = 0x17c;
    break;
  case 0x8149:
    sVar1 = 0x168;
    break;
  case 0x814a:
    sVar1 = 0x28;
    goto LAB_00201158;
  case 0x814b:
    sVar1 = 0x3c;
    goto LAB_00201158;
  case 0x815b:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = 0x168;
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)param_2));
LAB_002012e8:
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = sVar1 + 0x78;
    return 0;
  case 0x8160:
    sVar1 = 0x1cc;
    break;
  case 0x8175:
    sVar1 = 0x1e0;
    break;
  case 0x8176:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = 0;
    goto LAB_0020115c;
  case 0x817b:
    sVar1 = 0x8c;
    break;
  case 0x817c:
    sVar1 = 0xa0;
    break;
  case 0x817d:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = 0xdc;
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0x140;
    return 0;
  case 0x817e:
    sVar1 = 0xb4;
    goto LAB_002010ec;
  case 0x818b:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = 0xc0;
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0x60;
    return 0;
  case 0x8190:
    sVar1 = 0x14;
    goto LAB_00201158;
  case 0x8192:
    sVar1 = 0x50;
    goto LAB_00201158;
  case 0x8195:
    sVar1 = 0x78;
    break;
  case 0x8199:
    sVar1 = 100;
    break;
  case 0x819b:
    sVar1 = 0xa0;
    goto LAB_002010ec;
  case 0x81a0:
    sVar1 = 200;
    goto LAB_002010ec;
  case 0x81a2:
    sVar1 = 0xdc;
LAB_002010ec:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = sVar1;
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0xe0;
    return 0;
  case 0x81f4:
    sVar1 = 100;
LAB_00201158:
    (*(int16_t*)rtguest::G((uint32_t)param_1)) = sVar1;
LAB_0020115c:
    (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0xb4;
    return 0;
  }
  (*(int16_t*)rtguest::G((uint32_t)param_1)) = sVar1;
  (*(int16_t*)rtguest::G((uint32_t)param_2)) = (*(int16_t*)rtguest::G((uint32_t)param_2)) + 0xa0;
  return 0;
}


RT_GUEST_FN(0x00200fe8, guest::uiKanaGlyphUV)

}  // namespace guest
