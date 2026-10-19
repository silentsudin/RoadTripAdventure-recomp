// guest:0x0028b440 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceLseek64(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
  uint uVar1;
  long lVar2;
  undefined8 uVar3;
uint32_t piVar4;
  int iVar5;
uint32_t puVar6;
uint32_t auStack_b0 = rtguest::allocFrame(4);
uint32_t auStack_90 = rtguest::allocFrame(16);
  
  lVar2 = get_iob();
  _sceFsWaitS(0x16);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    _sceFsSigSema();
    uVar3 = 0xffffffffffffffff;
  }
  else {
    if (lVar2 != 0) {
      puVar6 = (uint32_t)lVar2;
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint32_t)));
      if (uVar1 != 0) {
        (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar6));
        (*(uint32_t*)rtguest::G((uint32_t)0x017788d8 + 4)) = (int)(puVar6 + -0x5de660) >> 4;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
        (*(uint32_t*)rtguest::G(0x17788d0)) = param_2;
        (*(uint32_t*)rtguest::G((uint32_t)0x017788d8 + 0)) = param_3;
        uVar3 = CreateSema(auStack_b0);
        (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_90;
        (*(uint32_t*)rtguest::G(0x17788c8)) = 8;
        (*(uint32_t*)rtguest::G(0x17788c0)) = (int)uVar3;
        if ((uVar1 & 0x8000) != 0) {
          WaitSema((*(uint32_t*)rtguest::G(0x2e9994)));
          if ((*(uint32_t*)rtguest::G(0x2e9908)) == -1) {
            (*(uint32_t*)rtguest::G(0x2e9908)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
          else {
            iVar5 = 1;
            do {
              if (0x1f < iVar5) goto LAB_0028b59c;
              piVar4 = 0x002e9908 + iVar5;
              iVar5 = iVar5 + 1;
            } while ((*(int32_t*)rtguest::G((uint32_t)piVar4)) != -1);
            (*(int32_t*)rtguest::G((uint32_t)piVar4)) = (*(uint32_t*)rtguest::G(0x17788c0));
            (*(uint32_t*)rtguest::G(0x17788c0)) = -(*(uint32_t*)rtguest::G(0x17788c0));
          }
LAB_0028b59c:
          SignalSema((*(uint32_t*)rtguest::G(0x2e9994)));
        }
        lVar2 = sceSifCallRpc(0x1779b80,0x16,0,0x17788c0,0x20,0x1779500,4,0);
        iVar5 = (*(uint32_t*)rtguest::G(0x21779500));
        if (lVar2 < 0) {
          DeleteSema(uVar3);
          _sceFsSigSema();
          return 0xfffffffffffffff5;
        }
        _sceFsSigSema();
        if (iVar5 == 0) {
          DeleteSema(uVar3);
          return 0xfffffffffffffff5;
        }
        if ((uVar1 & 0x8000) != 0) {
          DeleteSema(uVar3);
          return 0;
        }
        WaitSema(uVar3);
        DeleteSema(uVar3);

      }
    }
    _sceFsSigSema();
    uVar3 = 0xfffffffffffffff7;
  }
                    /* WARNING: Read-only address (ram,0x21779500) is written */
  return uVar3;
}


RT_GUEST_FN(0x0028b440, guest::sceLseek64)

}  // namespace guest
