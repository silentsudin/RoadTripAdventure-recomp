// guest:0x0028abc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceChstat(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(224);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xd4) = 0x00000000u;
  }
uint32_t auStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_bc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_b0 = rtguest::allocFrame(16);
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  long lVar4;
  int iVar5;
  
  RT_CALLRA(0x28ac08, guest::_sceFsWaitS(0xd));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28ac20, guest::sceFsInit());
    (*(uint32_t*)rtguest::G(0x1778910)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint32_t*)rtguest::G(0x1778910)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar2 = 0;
  cVar1 = (*(uint32_t*)rtguest::G(0x1778910));
  while (iVar5 = iVar2, cVar1 != '\0') {
    iVar5 = iVar2 + 1;
    if (0x3ff < iVar5) break;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar5) * sizeof(int8_t)));
    (*(uint32_t*)rtguest::G((uint32_t)0x1778911 + (uint32_t)(iVar2) * 4)) = cVar1;
    iVar2 = iVar5;
  }
  if (iVar5 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778d0f)) = 0;
    iVar5 = 0x3ff;
  }
  (*(uint32_t*)rtguest::G(0x17788d0)) = (*(uint64_t*)rtguest::G((uint32_t)param_2));
  (*(uint32_t*)rtguest::G(0x17788d8)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x17788e0)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x17788e8)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x17788f0)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x17788f8)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(5) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x1778900)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(6) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G(0x1778908)) = (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(7) * sizeof(uint64_t)));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
  (*(uint32_t*)rtguest::G(0x17788cc)) = param_3;
  uVar3 = RT_CALLRA(0x28ad2c, guest::CreateSema(auStack_d0));
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
  sceSifWriteBackDCache(0x17788c0,0x450);
  lVar4 = RT_CALLRA(0x28ad74, guest::sceSifCallRpc(0x1779b80,0xd,0,0x17788c0,iVar5 + 0x51,0x1779500,4,0));
  iVar2 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar4 < 0) {
    RT_CALLRA(0x28ad84, guest::DeleteSema(uVar3));
    RT_CALLRA(0x28ad8c, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28ada0, guest::_sceFsSigSema());
    if (iVar2 == 0) {
      RT_CALLRA(0x28adb0, guest::DeleteSema(uVar3));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28adc0, guest::WaitSema(uVar3));
      RT_CALLRA(0x28adc8, guest::DeleteSema(uVar3));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028abc8, guest::sceChstat)

}  // namespace guest
