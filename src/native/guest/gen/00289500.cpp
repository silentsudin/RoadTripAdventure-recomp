// guest:0x00289500 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceRead(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_bc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_b0 = rtguest::allocFrame(16);
  uint uVar1;
  undefined4 uVar2;
  long lVar3;
  undefined8 uVar4;
uint32_t piVar5;
  int iVar6;
uint32_t puVar7;
  
  lVar3 = get_iob();
  _sceFsWaitS(2);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    _sceFsSigSema();
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
        uVar4 = CreateSema(auStack_d0);
        (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
        (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
        (*(uint32_t*)rtguest::G(0x17788c0)) = (int)uVar4;
        if ((uVar1 & 0x8000) != 0) {
          WaitSema((*(uint32_t*)rtguest::G(0x2e9994)));
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
          SignalSema((*(uint32_t*)rtguest::G(0x2e9994)));
        }
        if ((uVar1 & 0x20000000) == 0) {
          sceSifWriteBackDCache(param_2,param_3);
        }
        sceSifWriteBackDCache(0x1779540,0xa4);
        sceSifWriteBackDCache(0x17788c0,0x20);
        lVar3 = sceSifCallRpc(0x1779b80,2,0,0x17788c0,0x20,0x1779500,4,0);
        iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
        if (lVar3 < 0) {DeleteSema(uVar4);
          return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));
_sceFsSigSema();
          return 0xfffffff5;
        }
        _sceFsSigSema();
        if (iVar6 == 0) {
          DeleteSema(uVar4);
          return 0xfffffff5;
        }
        if ((uVar1 & 0x8000) != 0) {
          DeleteSema(uVar4);
          return 0;
        }
        WaitSema(uVar4);
        DeleteSema(uVar4);

      }
    }
    _sceFsSigSema();
    uVar2 = 0xfffffff7;
  }
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return uVar2;
}


RT_GUEST_FN(0x00289500, guest::sceRead)

}  // namespace guest
