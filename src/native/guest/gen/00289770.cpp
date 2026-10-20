// guest:0x00289770 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceWrite(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t auStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_bc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_b0 = rtguest::allocFrame(16);
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
uint32_t puVar4;
  long lVar5;
  undefined8 uVar6;
uint32_t piVar7;
  int iVar8;
uint32_t puVar9;
  int iVar10;
  uint uVar11;
  
  lVar5 = get_iob();
  _sceFsWaitS(3);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    _sceFsSigSema();
    uVar3 = 0xffffffff;
  }
  else {
    if (lVar5 != 0) {
      puVar9 = (uint32_t)lVar5;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(1) * sizeof(uint32_t)));
      if (uVar1 != 0) {
        (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)0x017788e8 + 4)) = (int)(puVar9 + -0x5de660) >> 4;
        uVar11 = (uint32_t)param_2;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0;
        (*(uint32_t*)rtguest::G(0x17788d0)) = uVar11;
        (*(uint32_t*)rtguest::G(0x17788d4)) = param_3;
        uVar6 = CreateSema(auStack_d0);
        (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_b0;
        (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
        (*(uint32_t*)rtguest::G(0x17788c0)) = (int)uVar6;
        if ((uVar1 & 0x8000) != 0) {
          WaitSema((*(uint32_t*)rtguest::G(0x2e9994)));
          if ((*(uint32_t*)rtguest::G(0x2e9908)) == -1) {
            (*(uint32_t*)rtguest::G(0x2e9908)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
          else {
            iVar10 = 1;
            do {
              if (0x1f < iVar10) goto LAB_002898d4;
              piVar7 = 0x002e9908 + iVar10;
              iVar10 = iVar10 + 1;
            } while ((*(int32_t*)rtguest::G((uint32_t)piVar7)) != -1);
            (*(int32_t*)rtguest::G((uint32_t)piVar7)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
LAB_002898d4:
          SignalSema((*(uint32_t*)rtguest::G(0x2e9994)));
        }
        if ((param_2 & 0xf) == 0) {
          iVar10 = 0;
        }
        else {
          iVar10 = (uVar11 & 0xfffffff0) - (uVar11 - 0x10);
        }
        if (param_3 < iVar10) {
          iVar10 = param_3;
        }
        if ((uVar1 & 0x20000000) == 0) {
          sceSifWriteBackDCache(param_2,param_3);
        }
        iVar8 = 0;
        (*(uint32_t*)rtguest::G((uint32_t)0x017788d8 + 0)) = iVar10;
        if (0 < iVar10) {
          do {
            puVar4 = (uint32_t)((uVar11 | 0x20000000) + iVar8);
            iVar2 = iVar8 + 4;
            iVar8 = iVar8 + 1;
            (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)0x017788d8 + iVar2))) = (*(uint8_t*)rtguest::G((uint32_t)puVar4));
          } while (iVar8 < iVar10);
        }
        lVar5 = sceSifCallRpc(0x1779b80,3,0,0x17788c0,0x30,0x1779500,4,0);
        iVar10 = (*(uint32_t*)rtguest::G(0x21779500));
        if (lVar5 < 0) {DeleteSema(uVar6);
          return (*(uint32_t*)rtguest::G((uint32_t)auStack_b0 + (uint32_t)(0) * 4));
_sceFsSigSema();
          return 0xfffffff5;
        }
        _sceFsSigSema();
        if (iVar10 == 0) {
          DeleteSema(uVar6);
          return 0xfffffff5;
        }
        if ((uVar1 & 0x8000) != 0) {
          DeleteSema(uVar6);
          return 0;
        }
        WaitSema(uVar6);
        DeleteSema(uVar6);

      }
    }
    _sceFsSigSema();
    uVar3 = 0xfffffff7;
  }
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return uVar3;
}


RT_GUEST_FN(0x00289770, guest::sceWrite)

}  // namespace guest
