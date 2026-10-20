// guest:0x0028ae08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceRename(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
  }
uint32_t auStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_9c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_90 = rtguest::allocFrame(16);
  char cVar1;
  undefined8 uVar2;
  long lVar3;
  int iVar4;
  
  RT_CALLRA(0x28ae3c, guest::_sceFsWaitS(0x11));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28ae54, guest::sceFsInit());
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
    (*(uint32_t*)rtguest::G(0x1778ccb)) = 0;
  }
  (*(uint32_t*)rtguest::G(0x1778ccc)) = (*(int8_t*)rtguest::G((uint32_t)param_2));
  iVar4 = 0;
  if ((*(uint32_t*)rtguest::G(0x1778ccc)) != '\0') {
    for (iVar4 = 1;
        (iVar4 < 0x400 && (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(iVar4) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x1778ccc + (uint32_t)(iVar4) * 4)) = cVar1, cVar1 != '\0'));
        iVar4 = iVar4 + 1) {
    }
  }
  if (iVar4 == 0x400) {
    (*(uint32_t*)rtguest::G(0x17790cb)) = 0;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
  uVar2 = RT_CALLRA(0x28af24, guest::CreateSema(auStack_b0));
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_90;
  sceSifWriteBackDCache(0x17788c0,0x80c);
  lVar3 = RT_CALLRA(0x28af70, guest::sceSifCallRpc(0x1779b80,0x11,0,0x17788c0,0x80c,0x1779500,4,0));
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28af80, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28af88, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28af9c, guest::_sceFsSigSema());
    if (iVar4 == 0) {
      RT_CALLRA(0x28afac, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28afbc, guest::WaitSema(uVar2));
      RT_CALLRA(0x28afc4, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028ae08, guest::sceRename)

}  // namespace guest
