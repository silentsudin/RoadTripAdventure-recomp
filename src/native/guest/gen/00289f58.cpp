// guest:0x00289f58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceCallCode(uint32_t param_1, uint32_t param_2, ...) {
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
  char cVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  RT_CALLRA(0x289f90, guest::_sceFsWaitS(param_2));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x289fa8, guest::sceFsInit());
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar4 = 0;
  if ((char)(*(uint32_t*)rtguest::G(0x17788cc)) != '\0') {
    for (iVar4 = 1;
        (iVar4 < 0x400 &&
        (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar4) * sizeof(int8_t))), (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x017788cc + iVar4))) = cVar1, cVar1 != '\0'));
        iVar4 = iVar4 + 1) {
    }
  }
  if (iVar4 == 0x400) {
    (*(uint8_t*)rtguest::G(0x1778ccb)) = 0;  // MIPS sb 0x40b($18): byte store
    iVar4 = 0x3ff;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
  uVar2 = RT_CALLRA(0x28a034, guest::CreateSema(auStack_c0));
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
  lVar3 = RT_CALLRA(0x28a070, guest::sceSifCallRpc(0x1779b80,param_2,0,0x17788c0,iVar4 + 0xd,0x1779500,4,0));
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28a080, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28a088, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28a09c, guest::_sceFsSigSema());
    if (iVar4 == 0) {
      RT_CALLRA(0x28a0ac, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28a0bc, guest::WaitSema(uVar2));
      RT_CALLRA(0x28a0c4, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));
}


RT_GUEST_FN(0x00289f58, guest::_sceCallCode)

}  // namespace guest
