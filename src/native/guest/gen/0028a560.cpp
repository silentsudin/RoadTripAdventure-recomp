// guest:0x0028a560 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceAddDrv(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(144);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
uint32_t auStack_80 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0x80, frame 0x90)
    uint32_t uStack_7c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x7c, frame 0x90)
    uint32_t uStack_78 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0x78, frame 0x90)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_6c = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0x6c, frame 0x90)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_60 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0x60, frame 0x90)
  int iVar1;
  undefined8 uVar2;
  long lVar3;
  
  RT_CALLRA(0x28a584, guest::_sceFsWaitS(0xf));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28a59c, guest::sceFsInit());
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_78)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
  (*(uint32_t*)rtguest::G(0x17788cc)) = param_1;
  uVar2 = RT_CALLRA(0x28a5b8, guest::CreateSema(auStack_80));
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_60;
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  lVar3 = RT_CALLRA(0x28a604, guest::sceSifCallRpc(0x1779b80,0xf,0,0x17788c0,0x10,0x1779500,4,0));
  iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28a614, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28a61c, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xffffffff;
  }
  else {
    RT_CALLRA(0x28a630, guest::_sceFsSigSema());
    if (iVar1 == 0) {
      RT_CALLRA(0x28a640, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4)) = 0xffffffff;
    }
    else {
      RT_CALLRA(0x28a650, guest::WaitSema(uVar2));
      RT_CALLRA(0x28a658, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_60 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028a560, guest::sceAddDrv)

}  // namespace guest
