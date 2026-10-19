// guest:0x0020a8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReadPadState(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined1 uVar1;
  char cVar2;
  long lVar3;
  undefined8 uVar4;
  int iVar5;
  
  lVar3 = scePadGetState(param_2,0);
  iVar5 = (int)param_2;
  if (lVar3 == 0) {
    (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = 0;
    return 0;
  }
  if (lVar3 != 2) {
    if (lVar3 != 6) {
      return 0;
    }
    switch((*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1))) {
    case 0:
      scePadInfoMode(param_2,0,1,0);
      lVar3 = scePadInfoMode(param_2,0,2,0);
      if (lVar3 == 4) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 1;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 99;
      }
      break;
    case 1:
      lVar3 = scePadSetMainMode(param_2,0,1,1);
      if (lVar3 == 1) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) + '\x01';
      }
      break;
    case 2:
      lVar3 = scePadInfoAct(param_2,0,0xffffffffffffffff,0);
      if ((lVar3 == 2) && (lVar3 = scePadSetActAlign(param_2,0,0x333368), lVar3 != 0)) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 3;
      }
      break;
    case 3:
      uVar1 = scePadInfoMode(param_2,0,2,0);
      (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = uVar1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = uVar1;
      break;
    case 4:
    case 7:
      cVar2 = scePadInfoMode(param_2,0,2,0);
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = cVar2;
      if ((*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) == cVar2) {
        scePadSetActDirect(param_2,0,0x003356a8 + iVar5 * 2);
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = '\x02';
      }
      break;
    case 5:
    case 6:
    default:
      return 0;
    }
    uVar4 = scePadRead(param_2,0,param_3);
    return uVar4;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = 1;
  uVar4 = scePadRead(param_2,0,param_3);
  return uVar4;
}


RT_GUEST_FN(0x0020a8a8, guest::ReadPadState)

}  // namespace guest
