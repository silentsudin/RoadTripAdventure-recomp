// guest:0x0028ba88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceReadlink(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(208);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    rtguest::setS32(0, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[2] = 0x00000000017788c0ull;  // walk const
    rtguest::setS32(5, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[7] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_c0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xc0, frame 0xd0)
    uint32_t uStack_bc = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xbc, frame 0xd0)
    uint32_t uStack_b8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xb8, frame 0xd0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_ac = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0xac, frame 0xd0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_a0 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0xa0, frame 0xd0)
  char cVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  RT_CALLRA(0x28bac4, guest::_sceFsWaitS(0x11));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28badc, guest::sceFsInit());
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
  if (0x3ff < param_3) {
    param_3 = 0x3ff;
  }
  (*(uint32_t*)rtguest::G(0x17788d0)) = (uint32_t)param_2;
  (*(uint32_t*)rtguest::G(0x17788cc)) = (uint32_t)param_3;
  sceSifWriteBackDCache(param_2,param_3);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  uVar2 = RT_CALLRA(0x28bb88, guest::CreateSema(auStack_c0));
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
  lVar3 = RT_CALLRA(0x28bbc4, guest::sceSifCallRpc(0x1779b80,0x19,0,0x17788c0,0x80c,0x1779500,4,0));
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28bbd4, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28bbdc, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28bbf0, guest::_sceFsSigSema());
    if (iVar4 == 0) {
      RT_CALLRA(0x28bc00, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28bc10, guest::WaitSema(uVar2));
      RT_CALLRA(0x28bc18, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028ba88, guest::sceReadlink)

}  // namespace guest
