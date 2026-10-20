// guest:0x0028b018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSync(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
  }
uint32_t auStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_9c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_90 = rtguest::allocFrame(16);
  char cVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  RT_CALLRA(0x28b04c, guest::_sceFsWaitS(0x13));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28b064, guest::sceFsInit());
    (*(uint32_t*)rtguest::G(0x17788d4)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788d4)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar4 = 0;
  if ((*(uint32_t*)rtguest::G(0x17788d4)) != '\0') {
    for (iVar4 = 1;
        (iVar4 < 0x400 && (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x17788d4 + (uint32_t)(iVar4) * 4)) = cVar1, cVar1 != '\0'));
        iVar4 = iVar4 + 1) {
    }
  }
  if (iVar4 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778cd3)) = 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
  (*(uint32_t*)rtguest::G(0x17788d0)) = param_2;
  uVar2 = RT_CALLRA(0x28b0ec, guest::CreateSema(auStack_b0));
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_90;
  lVar3 = RT_CALLRA(0x28b128, guest::sceSifCallRpc(0x1779b80,0x13,0,0x17788c0,0x414,0x1779500,4,0));
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28b138, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28b140, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28b154, guest::_sceFsSigSema());
    if (iVar4 == 0) {
      RT_CALLRA(0x28b164, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28b174, guest::WaitSema(uVar2));
      RT_CALLRA(0x28b17c, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028b018, guest::sceSync)

}  // namespace guest
