// guest:0x0028a8d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDread(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_80 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0x80, frame 0x90)
    uint32_t uStack_7c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x7c, frame 0x90)
    uint32_t uStack_78 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0x78, frame 0x90)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_6c = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0x6c, frame 0x90)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_60 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0x60, frame 0x90)
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
