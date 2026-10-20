// guest:0x00289d80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceIoctl2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(16);
  int iVar1;
  long lVar2;
  undefined8 uVar3;
  
  lVar2 = get_iob();
  _sceFsWaitS(0x1a);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
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
      uVar3 = CreateSema(auStack_c0);
      (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
      (*(uint32_t*)rtguest::G(0x1778cd8)) = (uint32_t)param_6;
      (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
      (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
      (*(uint32_t*)rtguest::G(0x1778cd4)) = param_5;
      sceSifWriteBackDCache(0x17788c0,0x420);
      lVar2 = sceSifCallRpc(0x1779b80,0x1a,0,0x17788c0,0x420,0x1779500,4,0);
      iVar1 = (*(uint32_t*)rtguest::G(0x21779500));
      if (lVar2 < 0) {DeleteSema(uVar3);
        return (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4));
_sceFsSigSema();
        return 0xfffffff5;
      }
      _sceFsSigSema();
      if (iVar1 == 0) {
        DeleteSema(uVar3);
        return 0xfffffff5;
      }
      WaitSema(uVar3);
      DeleteSema(uVar3);

    }
  }
  _sceFsSigSema();
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return 0xfffffff7;
}


RT_GUEST_FN(0x00289d80, guest::sceIoctl2)

}  // namespace guest
