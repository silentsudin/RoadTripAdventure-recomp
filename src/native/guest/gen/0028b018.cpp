// guest:0x0028b018 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSync(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::gS[0] = 0x00000000017788c0ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[6] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_b0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xb0, frame 0xc0)
    uint32_t uStack_ac = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xac, frame 0xc0)
    uint32_t uStack_a8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xa8, frame 0xc0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_9c = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0x9c, frame 0xc0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_90 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0x90, frame 0xc0)
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
