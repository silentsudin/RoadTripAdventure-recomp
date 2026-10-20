// guest:0x0028a8d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDread(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
uint32_t auStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    uint32_t uStack_78 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_6c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_60 = rtguest::allocFrame(16);
  int iVar1;
  long lVar2;
  undefined8 uVar3;
  
  lVar2 = RT_CALLRA(0x28a8f0, guest::get_iob());
  RT_CALLRA(0x28a8fc, guest::_sceFsWaitS(0xb));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28a914, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xffffffff;
  }
  else if ((lVar2 == 0) || ((*(uint32_t*)rtguest::G((uint32_t)lVar2 + (uint32_t)(1) * 4)) == 0)) {
    RT_CALLRA(0x28a938, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xfffffff7;
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)lVar2));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
    (*(uint32_t*)rtguest::G(0x17788d0)) = param_2;
    uVar3 = RT_CALLRA(0x28a960, guest::CreateSema(auStack_80));
    (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_60;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    lVar2 = RT_CALLRA(0x28a9ac, guest::sceSifCallRpc(0x1779b80,0xb,0,0x17788c0,0x20,0x1779500,4,0));
    iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
    if (lVar2 < 0) {
      RT_CALLRA(0x28a9bc, guest::WaitSema(uVar3));
      RT_CALLRA(0x28a9c4, guest::_sceFsSigSema());
      (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28a9d8, guest::_sceFsSigSema());
      if (iVar1 == 0) {
        RT_CALLRA(0x28a9e8, guest::DeleteSema(uVar3));
        (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xfffffff5;
      }
      else {
        RT_CALLRA(0x28a9f8, guest::WaitSema(uVar3));
        RT_CALLRA(0x28aa00, guest::DeleteSema(uVar3));
      }
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028a8d0, guest::sceDread)

}  // namespace guest
