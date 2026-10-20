// guest:0x0026d880 transliterated from Ghidra pseudocode — DO NOT hand-edit
// the (float)param_2[4] render is an lwc1 bit-reinterpret at +0x10 (door scale float).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void PrintDoorParams(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  int iVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  int iVar4;
  int iVar5;
  uint32_t auStack_40 = rtguest::allocFrame(32);

  uVar2 = RT_CALLRA(0x26d8a0, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_2)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335650)), uVar2);
  RT_CALLRA(0x26d8c8, guest::uiDebugText8(0x10, 0x50, auStack_40, 0xf));
  uVar2 = RT_CALLRA(0x26d8d4, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_2 + 4)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335658)), uVar2);
  RT_CALLRA(0x26d8fc, guest::uiDebugText8(0x10, 0x58, auStack_40, 0xf));
  uVar2 = RT_CALLRA(0x26d908, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_2 + 8)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335660)), uVar2);
  RT_CALLRA(0x26d930, guest::uiDebugText8(0x10, 0x60, auStack_40, 0xf));
  uVar2 = RT_CALLRA(0x26d93c, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_2 + 0x10)))));
  uVar3 = RT_CALLRA(0x26d970, guest::fptodp(rtguest::f2bits((*(float*)rtguest::G((uint32_t)(param_2 + 0x10))) * 18.0f / 320.0f)));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb48)), uVar2, uVar3);
  RT_CALLRA(0x26d998, guest::uiDebugText8(0x10, 0x68, auStack_40, 0xf));
  iVar1 = ((int)(*(int16_t*)rtguest::G((uint32_t)(param_2 + 0x14)))) * 0xb4;
  iVar4 = ((int)(*(int16_t*)rtguest::G((uint32_t)(param_2 + 0x16)))) * 0xb4;
  iVar5 = ((int)(*(int16_t*)rtguest::G((uint32_t)(param_2 + 0x18)))) * 0xb4;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0x7fff;
  }
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0x7fff;
  }
  if (iVar5 < 0) {
    iVar5 = iVar5 + 0x7fff;
  }
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb58)),
            iVar1 >> 0xf, iVar4 >> 0xf, iVar5 >> 0xf);
  RT_CALLRA(0x26da40, guest::uiDebugText8(0x10, 0x70, auStack_40, 0xf));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x32fb68)),
            (*(uint16_t*)rtguest::G((uint32_t)(param_2 + 0x14))),
            (*(uint16_t*)rtguest::G((uint32_t)(param_2 + 0x16))),
            (*(uint16_t*)rtguest::G((uint32_t)(param_2 + 0x18))));
  RT_CALLRA(0x26da70, guest::uiDebugText8(0x20, 0x78, auStack_40, 0xf));
  return;
}


RT_GUEST_FN(0x0026d880, guest::PrintDoorParams)

}  // namespace guest
