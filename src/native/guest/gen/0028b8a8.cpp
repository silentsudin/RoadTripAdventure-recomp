// guest:0x0028b8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSymlink(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(192);
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xb0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xb4) = 0x00000000u;
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
  
  RT_CALLRA(0x28b8dc, guest::_sceFsWaitS(0x11));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28b8f4, guest::sceFsInit());
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
  uVar2 = RT_CALLRA(0x28b9c4, guest::CreateSema(auStack_b0));
  (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar2;
  (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
  (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_90;
  lVar3 = RT_CALLRA(0x28ba00, guest::sceSifCallRpc(0x1779b80,0x18,0,0x17788c0,0x80c,0x1779500,4,0));
  iVar4 = (*(uint32_t*)rtguest::G(0x21779500));
  if (lVar3 < 0) {
    RT_CALLRA(0x28ba10, guest::DeleteSema(uVar2));
    RT_CALLRA(0x28ba18, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
  }
  else {
    RT_CALLRA(0x28ba2c, guest::_sceFsSigSema());
    if (iVar4 == 0) {
      RT_CALLRA(0x28ba3c, guest::DeleteSema(uVar2));
      (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28ba4c, guest::WaitSema(uVar2));
      RT_CALLRA(0x28ba54, guest::DeleteSema(uVar2));
    }
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_90 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028b8a8, guest::sceSymlink)

}  // namespace guest
