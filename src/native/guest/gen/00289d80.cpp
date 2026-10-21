// guest:0x00289d80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceIoctl2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(208);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
  }
uint32_t auStack_c0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xc0, frame 0xd0)
    uint32_t uStack_bc = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xbc, frame 0xd0)
    uint32_t uStack_b8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xb8, frame 0xd0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_ac = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0xac, frame 0xd0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_a0 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0xa0, frame 0xd0)
  int iVar1;
  long lVar2;
  undefined8 uVar3;
  
  lVar2 = RT_CALLRA(0x289dc0, guest::get_iob());
  RT_CALLRA(0x289dcc, guest::_sceFsWaitS(0x1a));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x289de4, guest::sceFsInit());
  }
  if (lVar2 != 0) {
    if ((*(uint32_t*)rtguest::G((uint32_t)lVar2 + (uint32_t)(1) * 4)) != 0) {
      if ((0x400 < param_4) || (0x400 < param_6)) {
        return 0xffffffea;
      }
      if (param_3 != 0) {
        (uint32_t)(uintptr_t)(::memcpy((char *)rtguest::G((uint32_t)(0x017788d4)), (char *)rtguest::G((uint32_t)((uint32_t)param_3)), param_4));
      }
      (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)lVar2));
      (*(uint32_t*)rtguest::G(0x1778cdc)) = (uint32_t)param_4;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
      (*(uint32_t*)rtguest::G(0x17788d0)) = param_2;
      uVar3 = RT_CALLRA(0x289e68, guest::CreateSema(auStack_c0));
      (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
      (*(uint32_t*)rtguest::G(0x1778cd8)) = (uint32_t)param_6;
      (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
      (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
      (*(uint32_t*)rtguest::G(0x1778cd4)) = param_5;
      sceSifWriteBackDCache(0x17788c0,0x420);
      lVar2 = RT_CALLRA(0x289ec8, guest::sceSifCallRpc(0x1779b80,0x1a,0,0x17788c0,0x420,0x1779500,4,0));
      iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
      if (lVar2 < 0) {RT_CALLRA(0x289ed8, guest::DeleteSema(uVar3));
        return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));
RT_CALLRA(0x289e00, guest::_sceFsSigSema());
        return 0xfffffff5;
      }
      RT_CALLRA(0x289ee0, guest::_sceFsSigSema());
      if (iVar1 == 0) {
        RT_CALLRA(0x289f04, guest::DeleteSema(uVar3));
        return 0xfffffff5;
      }
      RT_CALLRA(0x289f14, guest::WaitSema(uVar3));
      RT_CALLRA(0x289f1c, guest::DeleteSema(uVar3));

    }
  }
  RT_CALLRA(0x289ef4, guest::_sceFsSigSema());
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return 0xfffffff7;
}


RT_GUEST_FN(0x00289d80, guest::sceIoctl2)

}  // namespace guest
