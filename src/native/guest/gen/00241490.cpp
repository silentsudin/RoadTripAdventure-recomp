// guest:0x00241490 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DispatchCourseEffect(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
  
  InitCarRenderRegs();
  iVar2 = (int)param_1;
  switch((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)))) {
  case 0:
  case 1:
  case 6:
  case 8:
  case 9:
  case 0xb:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1e:
  case 0x20:
  case 0x21:
  case 0x23:
  case 0x24:
  default:
    gs_tasks_kill(param_1);
    return 0;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23e140;
    EmptyStub2(param_1);
    return 0;
  case 3:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x241688;
    InitializeEffectPackets();
    iVar2 = 0;
    do {
      iVar1 = iVar2 + 1;
      gs_tasks_spawn(param_1,0x241f68,iVar2);
      iVar2 = iVar1;
    } while (iVar1 < 6);
    break;
  case 4:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x241760;
    EmptyStub2(param_1);
    return 0;
  case 5:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23e508;
    EmptyStub2(param_1);
    return 0;
  case 7:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x241838;
    EmptyStub2(param_1);
    return 0;
  case 10:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23e7e0;
    EmptyStub2(param_1);
    return 0;
  case 0xc:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23e8d0;
    EmptyStub2(param_1);
    return 0;
  case 0xd:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23e9c0;
    EmptyStub2(param_1);
    return 0;
  case 0xe:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23eca0;
    EmptyStub2(param_1);
    return 0;
  case 0x13:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x241ba0;
    InitializeEffectPackets();
    iVar2 = 6;
    do {
      iVar1 = iVar2 + 1;
      gs_tasks_spawn(param_1,0x241f68,iVar2);
      iVar2 = iVar1;
    } while (iVar1 < 0xd);
    break;
  case 0x14:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x241840;
    EmptyStub2(param_1);
    return 0;
  case 0x1d:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23ee70;
    EmptyStub2(param_1);
    return 0;
  case 0x1f:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23efb0;
    EmptyStub2(param_1);
    return 0;
  case 0x22:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23f0a0;
    EmptyStub2(param_1);
    return 0;
  case 0x25:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x23f398;
    EmptyStub2(param_1);
    return 0;
  case 0xff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) = 0x2412e0;
    EmptyStub2();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00241490, guest::DispatchCourseEffect)

}  // namespace guest
