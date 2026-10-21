// guest:0x00289500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceRead(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(224);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0xb0, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0xa0, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xd4) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xc0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x60, 8);
    rtguest::gS[1] = 0x00000000017788c0ull;  // walk const
    rtguest::gS[4] = 0x0000000001780000ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(7, (uint32_t)param_3);  // walk: = param_3
  }
uint32_t auStack_d0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xd0, frame 0xe0)
    uint32_t uStack_cc = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xcc, frame 0xe0)
    uint32_t uStack_c8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xc8, frame 0xe0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_bc = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0xbc, frame 0xe0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_b0 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0xb0, frame 0xe0)
  uint uVar1;
  undefined4 uVar2;
  long lVar3;
  undefined8 uVar4;
uint32_t piVar5;
  int iVar6;
uint32_t puVar7;
  
  lVar3 = RT_CALLRA(0x289538, guest::get_iob());
  RT_CALLRA(0x289544, guest::_sceFsWaitS(2));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28955c, guest::_sceFsSigSema());
    uVar2 = 0xffffffff;
  }
  else {
    if (lVar3 != 0) {
      puVar7 = (uint32_t)lVar3;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint32_t)));
      if (uVar1 != 0) {
        (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x017788d8 + 4)) = (int)(puVar7 + -0x5de660) >> 4;
        (*(uint32_t*)rtguest::G(0x17788d0)) = (uint32_t)param_2;
        (*(uint32_t*)rtguest::G(0x17788d4)) = (uint32_t)param_3;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
        uVar4 = RT_CALLRA(0x2895c0, guest::CreateSema(auStack_d0));
        (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
        (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
        (*(uint32_t*)rtguest::G(0x17788c0)) = (int)uVar4;
        if ((uVar1 & 0x8000) != 0) {
          RT_CALLRA(0x2895ec, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9994))));
          if ((*(uint32_t*)rtguest::G(0x2e9908)) == -1) {
            (*(uint32_t*)rtguest::G(0x2e9908)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
          else {
            iVar6 = 1;
            do {
              if (0x1f < iVar6) goto LAB_00289658;
              piVar5 = 0x002e9908 + iVar6;
              iVar6 = iVar6 + 1;
            } while ((*(int32_t*)rtguest::G((uint32_t)piVar5)) != -1);
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
LAB_00289658:
          RT_CALLRA(0x289658, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9994))));
        }
        if ((uVar1 & 0x20000000) == 0) {
          sceSifWriteBackDCache(param_2,param_3);
        }
        sceSifWriteBackDCache(0x1779540,0xa4);
        sceSifWriteBackDCache(0x17788c0,0x20);
        lVar3 = RT_CALLRA(0x2896cc, guest::sceSifCallRpc(0x1779b80,2,0,0x17788c0,0x20,0x1779500,4,0));
        iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
        if (lVar3 < 0) {RT_CALLRA(0x2896dc, guest::DeleteSema(uVar4));
          return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));
RT_CALLRA(0x289580, guest::_sceFsSigSema());
          return 0xfffffff5;
        }
        RT_CALLRA(0x2896e4, guest::_sceFsSigSema());
        if (iVar6 == 0) {
          RT_CALLRA(0x289704, guest::DeleteSema(uVar4));
          return 0xfffffff5;
        }
        if ((uVar1 & 0x8000) != 0) {
          RT_CALLRA(0x28971c, guest::DeleteSema(uVar4));
          return 0;
        }
        RT_CALLRA(0x28972c, guest::WaitSema(uVar4));
        RT_CALLRA(0x289734, guest::DeleteSema(uVar4));

      }
    }
    RT_CALLRA(0x2896f4, guest::_sceFsSigSema());
    uVar2 = 0xfffffff7;
  }
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return uVar2;
}


RT_GUEST_FN(0x00289500, guest::sceRead)

}  // namespace guest
