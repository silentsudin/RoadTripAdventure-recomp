// guest:0x0020a8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReadPadState(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  undefined1 uVar1;
  char cVar2;
  long lVar3;
  undefined8 uVar4;
  int iVar5;
  
  lVar3 = RT_CALLRA(0x20a8cc, guest::scePadGetState(param_2,0));
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
      RT_CALLRA(0x20a974, guest::scePadInfoMode(param_2,0,1,0));
      lVar3 = RT_CALLRA(0x20a988, guest::scePadInfoMode(param_2,0,2,0));
      if (lVar3 == 4) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 1;
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 99;
      }
      break;
    case 1:
      lVar3 = RT_CALLRA(0x20a9cc, guest::scePadSetMainMode(param_2,0,1,1));
      if (lVar3 == 1) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) + '\x01';
      }
      break;
    case 2:
      lVar3 = RT_CALLRA(0x20aa04, guest::scePadInfoAct(param_2,0,0xffffffffffffffff,0));
      if ((lVar3 == 2) && (lVar3 = scePadSetActAlign(param_2,0,0x333368), lVar3 != 0)) {
        (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = 3;
      }
      break;
    case 3:
      uVar1 = RT_CALLRA(0x20aa4c, guest::scePadInfoMode(param_2,0,2,0));
      (*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) = uVar1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = uVar1;
      break;
    case 4:
    case 7:
      cVar2 = RT_CALLRA(0x20aa74, guest::scePadInfoMode(param_2,0,2,0));
      (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + iVar5 + 0xc))) = cVar2;
      if ((*(uint8_t*)rtguest::G((uint32_t)0x333360 + (uint32_t)(iVar5) * 1)) == cVar2) {
        RT_CALLRA(0x20aab8, guest::scePadSetActDirect(param_2,0,0x003356a8 + iVar5 * 2));
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
