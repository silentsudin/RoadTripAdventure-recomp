// guest:0x00289148 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceClose(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
  }
uint32_t auStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t aiStack_70 = rtguest::allocFrame(16);
  int iVar1;
  long lVar2;
  undefined8 uVar3;
uint32_t puVar4;
  
  lVar2 = RT_CALLRA(0x289168, guest::get_iob());
  RT_CALLRA(0x289174, guest::_sceFsWaitS(1));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28918c, guest::_sceFsSigSema());
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -1;
  }
  else if ((lVar2 == 0) || (puVar4 = (uint32_t)lVar2, (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) == 0)) {
    RT_CALLRA(0x2891b0, guest::_sceFsSigSema());
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -9;
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 1;
    (*(uint32_t*)rtguest::G(0x17788d0)) = (int)(puVar4 + -0x5de660) >> 4;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0;
    uVar3 = RT_CALLRA(0x2891e8, guest::CreateSema(auStack_90));
    (*(uint32_t*)rtguest::G(0x17788c4)) = aiStack_70;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    lVar2 = RT_CALLRA(0x289234, guest::sceSifCallRpc(0x1779b80,1,0,0x17788c0,0x14,0x1779500,4,0));
    if (lVar2 < 0) {
      RT_CALLRA(0x289244, guest::DeleteSema(uVar3));
      RT_CALLRA(0x28924c, guest::_sceFsSigSema());
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
      iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
      RT_CALLRA(0x289264, guest::_sceFsSigSema());
      if (iVar1 == 0) {
        RT_CALLRA(0x289274, guest::DeleteSema(uVar3));
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
      }
      else {
        RT_CALLRA(0x289284, guest::WaitSema(uVar3));
        RT_CALLRA(0x28928c, guest::DeleteSema(uVar3));
        if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4))) {
          (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = 0;
        }
      }
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x00289148, guest::sceClose)

}  // namespace guest
