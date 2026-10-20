// guest:0x0028b1b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceMount(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(224);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xd0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xd4) = 0x00000000u;
  }
rtguest::allocFrame(16);  // guest frame gap (sp+0x0..0xf)
uint32_t auStack_d0 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0xd0, frame 0xe0)
    uint32_t uStack_cc = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0xcc, frame 0xe0)
    uint32_t uStack_c8 = rtguest::allocFrame(4);  // sp+0x18 (Ghidra entry-sp -0xc8, frame 0xe0)
    rtguest::allocFrame(8);  // guest frame gap (sp+0x1c..0x23)
    uint32_t uStack_bc = rtguest::allocFrame(4);  // sp+0x24 (Ghidra entry-sp -0xbc, frame 0xe0)
rtguest::allocFrame(8);  // guest frame gap (sp+0x28..0x2f)
uint32_t auStack_b0 = rtguest::allocFrame(16);  // sp+0x30 (Ghidra entry-sp -0xb0, frame 0xe0)
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t puVar5;
  int iVar6;
  
  RT_CALLRA(0x28b1f8, guest::_sceFsWaitS(0x14));
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    RT_CALLRA(0x28b210, guest::sceFsInit());
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  else {
    (*(uint8_t*)rtguest::G((uint32_t)0x017788cc + 0)) = (*(int8_t*)rtguest::G((uint32_t)param_1));
  }
  iVar6 = 0;
  if ((char)(*(uint32_t*)rtguest::G(0x17788cc)) != '\0') {
    for (iVar6 = 1;
        (iVar6 < 0x400 &&
        (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x017788cc + iVar6))) = cVar1, cVar1 != '\0'));
        iVar6 = iVar6 + 1) {
    }
  }
  if (iVar6 == 0x400) {
    (*(uint32_t*)rtguest::G(0x1778ccb)) = 0;
  }
  (*(uint32_t*)rtguest::G(0x1778ccc)) = (*(int8_t*)rtguest::G((uint32_t)param_2));
  iVar6 = 0;
  if ((*(uint32_t*)rtguest::G(0x1778ccc)) != '\0') {
    for (iVar6 = 1;
        (iVar6 < 0x400 && (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(uint32_t*)rtguest::G((uint32_t)0x1778ccc + (uint32_t)(iVar6) * 4)) = cVar1, cVar1 != '\0'));
        iVar6 = iVar6 + 1) {
    }
  }
  if (iVar6 == 0x400) {
    (*(uint32_t*)rtguest::G(0x17790cb)) = 0;
  }
  if (param_5 < 0x401) {
    iVar6 = 0;
    if (0 < param_5) {
      do {
        puVar2 = (uint32_t)(param_4 + iVar6);
        puVar5 = (uint32_t)((int)0x017790cc + iVar6);
        iVar6 = iVar6 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar5)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
      } while (iVar6 < param_5);
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
    (*(uint32_t*)rtguest::G(0x17794cc)) = param_3;
    (*(uint32_t*)rtguest::G(0x17794d0)) = param_5;
    uVar3 = RT_CALLRA(0x28b344, guest::CreateSema(auStack_d0));
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
    sceSifWriteBackDCache(0x17788c0,0xc14);
    lVar4 = RT_CALLRA(0x28b38c, guest::sceSifCallRpc(0x1779b80,0x14,0,0x17788c0,0xc14,0x1779500,4,0));
    iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
    if (lVar4 < 0) {
      RT_CALLRA(0x28b39c, guest::DeleteSema(uVar3));
      RT_CALLRA(0x28b2c8, guest::_sceFsSigSema());
      (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      RT_CALLRA(0x28b3a4, guest::_sceFsSigSema());
      if (iVar6 == 0) {
        RT_CALLRA(0x28b3c8, guest::DeleteSema(uVar3));
        (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff5;
      }
      else {
        RT_CALLRA(0x28b3d8, guest::WaitSema(uVar3));
        RT_CALLRA(0x28b3e0, guest::DeleteSema(uVar3));
      }
    }
  }
  else {
    RT_CALLRA(0x28b3b8, guest::_sceFsSigSema());
    (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4)) = 0xfffffff9;
  }
  return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));;
}


RT_GUEST_FN(0x0028b1b0, guest::sceMount)

}  // namespace guest
