// guest:0x0028a768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceDclose(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(160);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x90) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x94) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::gS[2] = 0x00000000017788c0ull;  // walk const
    rtguest::gS[4] = 0x0000000001780000ull;  // walk const
  }
uint32_t auStack_90 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0x90, frame 0xa0)
    uint32_t uStack_8c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x8c, frame 0xa0)
    uint32_t uStack_88 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0x88, frame 0xa0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_7c = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0x7c, frame 0xa0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t aiStack_70 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0x70, frame 0xa0)
  int iVar1;
  long lVar2;
  undefined8 uVar3;
uint32_t puVar4;
  
  lVar2 = RT_CALLRA(0x28a788, guest::get_iob());
  RT_CALLRA(0x28a794, guest::_sceFsWaitS(10));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28a7ac, guest::_sceFsSigSema());
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -1;
  }
  else if ((lVar2 == 0) || (puVar4 = (uint32_t)lVar2, (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) == 0)) {
    RT_CALLRA(0x28a7d0, guest::_sceFsSigSema());
    (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -9;
  }
  else {
    (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0;
    uVar3 = RT_CALLRA(0x28a7f4, guest::CreateSema(auStack_90));
    (*(uint32_t*)rtguest::G(0x17788c4)) = aiStack_70;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    lVar2 = RT_CALLRA(0x28a840, guest::sceSifCallRpc(0x1779b80,10,0,0x17788c0,0x14,0x1779500,4,0));
    if (lVar2 < 0) {
      RT_CALLRA(0x28a850, guest::DeleteSema(uVar3));
      RT_CALLRA(0x28a858, guest::_sceFsSigSema());
      (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
      iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
      RT_CALLRA(0x28a870, guest::_sceFsSigSema());
      if (iVar1 == 0) {
        RT_CALLRA(0x28a880, guest::DeleteSema(uVar3));
        (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = -0xb;
      }
      else {
        RT_CALLRA(0x28a890, guest::WaitSema(uVar3));
        RT_CALLRA(0x28a898, guest::DeleteSema(uVar3));
        if (-1 < (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4))) {
          (*(int32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4)) = 0;
        }
      }
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)aiStack_70 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028a768, guest::sceDclose)

}  // namespace guest
