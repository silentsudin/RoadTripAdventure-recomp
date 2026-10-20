// guest:0x00288ec0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceOpen(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(336);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xd4) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x120) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0x124) = (uint32_t)((int64_t)(int32_t)param_3 >> 32);
  }
uint32_t auStack_140 = rtguest::allocFrame(4);
    uint32_t uStack_13c = rtguest::allocFrame(4);
    uint32_t uStack_138 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_12c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t aiStack_120 = rtguest::allocFrame(16);
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  long lVar4;
  undefined8 uVar5;
  int iVar6;
uint32_t piVar7;
  
  RT_CALLRA(0x288f14, guest::_sceFsWaitS(0));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x288f2c, guest::sceFsInit());
  }
  lVar4 = RT_CALLRA(0x288f34, guest::_fs_version());
  if (lVar4 == 0) {
    lVar4 = RT_CALLRA(0x288f58, guest::fsAllocFd());
    if (lVar4 == 0) {
      RT_CALLRA(0x288f44, guest::_sceFsSigSema());
      iVar6 = -0x13;
    }
    else {
      (*(uint32_t*)rtguest::G(0x17788d4)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
      iVar6 = 0;
      if ((*(uint32_t*)rtguest::G(0x17788d4)) != '\0') {
        for (iVar6 = 1;
            (iVar6 < 0x400 &&
            (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x17788d4 + (uint32_t)(iVar6) * 4)) = cVar1, cVar1 != '\0'));
            iVar6 = iVar6 + 1) {
        }
      }
      if (iVar6 == 0x400) {
        (*(uint32_t*)rtguest::G(0x1778cd3)) = 0;
      }
      piVar7 = (uint32_t)lVar4;
      iVar6 = (int)(piVar7 + -0x5de660) >> 4;
      (*(uint32_t*)rtguest::G(0x17788cc)) = param_2 & 0x6fffffff;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_13c)) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_138)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_12c)) = 0;
      (*(uint32_t*)rtguest::G(0x17788d0)) = param_3;
      (*(uint32_t*)rtguest::G(0x1778cd4)) = iVar6;
      uVar5 = RT_CALLRA(0x289028, guest::CreateSema(auStack_140));
      (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar5;
      (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
      (*(uint32_t*)rtguest::G(0x17788c4)) = aiStack_120;
      lVar4 = RT_CALLRA(0x289064, guest::sceSifCallRpc(0x1779b80,0,0,0x17788c0,0x418,0x1779500,4,0));
      iVar3 = (*(uint32_t*)rtguest::G(0x21779500));
      if (lVar4 < 0) {
        RT_CALLRA(0x289074, guest::DeleteSema(uVar5));
        RT_CALLRA(0x288f6c, guest::_sceFsSigSema());
        iVar6 = -0xb;
      }
      else {
        RT_CALLRA(0x28907c, guest::_sceFsSigSema());
        if (iVar3 == 0) {
          RT_CALLRA(0x2890a0, guest::DeleteSema(uVar5));
          iVar6 = -0xb;
        }
        else {
          RT_CALLRA(0x2890b0, guest::WaitSema(uVar5));
          RT_CALLRA(0x2890b8, guest::DeleteSema(uVar5));
          if ((*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4)) < 0) {
            RT_CALLRA(0x2890cc, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
            (*(int32_t*)rtguest::G((uint32_t)piVar7 + (uint32_t)(1) * sizeof(int32_t))) = 0;
            RT_CALLRA(0x2890d8, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
            iVar6 = (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4));
          }
          else {
            RT_CALLRA(0x2890ec, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
            uVar2 = (*(uint32_t*)rtguest::G(0x2e9990));
            (*(int32_t*)rtguest::G((uint32_t)piVar7 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)piVar7 + (uint32_t)(1) * sizeof(int32_t))) | param_2;
            (*(int32_t*)rtguest::G((uint32_t)piVar7)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(0) * 4));
            RT_CALLRA(0x289108, guest::SignalSema(uVar2));
          }
        }
      }
    }
  }
  else {
    RT_CALLRA(0x289090, guest::_sceFsSigSema());
    iVar6 = -0x10004;
  }
  return iVar6;
}


RT_GUEST_FN(0x00288ec0, guest::sceOpen)

}  // namespace guest
