// guest:0x00289a30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceIoctl(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
uint32_t puVar1;
  long lVar2;
  undefined8 uVar3;
uint32_t piVar4;
  int iVar5;
uint32_t puVar6;
  undefined8 uVar7;
  undefined8 uVar8;
uint32_t puVar9;
uint32_t auStack_b0 = rtguest::allocFrame(4);
uint32_t auStack_90 = rtguest::allocFrame(16);
  
  lVar2 = get_iob();
  _sceFsWaitS(5);
  (*(uint32_t*)rtguest::G(0x1778884)) = param_3;
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
  }
  if ((lVar2 == 0) || (puVar9 = (uint32_t)lVar2, (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(1) * sizeof(uint32_t))) == 0)) {
    _sceFsSigSema();
    return 0xfffffff7;
  }
  (*(uint32_t*)rtguest::G(0x1778cd4)) = 0;
  (*(uint32_t*)rtguest::G(0x1778cd8)) = 0;
  if (param_2 == 2) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_3)) = (*(uint32_t*)rtguest::G(0x21779550));
  }
  else if (param_2 < 3) {
    if (param_2 != 1) {
      (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
LAB_00289ba8:
      (*(uint32_t*)rtguest::G(0x17788d0)) = (uint32_t)param_2;
      if (param_3 == (uint32_t)0x0) {
        (*(uint32_t*)rtguest::G(0x1778cdc)) = 0;
      }
      else {
        puVar6 = (uint32_t)0x017788d4;
        (*(uint32_t*)rtguest::G(0x1778cdc)) = 0x400;
        if ((((uint32_t)param_3 | 0x17788d4) & 7) == 0) {
          puVar1 = param_3 + 0x80;
          do {
            uVar3 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t)));
            uVar7 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t)));
            uVar8 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint64_t)));
            (*(uint64_t*)rtguest::G((uint32_t)puVar6)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t))) = uVar3;
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar7;
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
            param_3 = param_3 + 4;
            puVar6 = puVar6 + 4;
          } while (param_3 != puVar1);
        }
        else {
          puVar1 = param_3 + 0x80;
          do {
            uVar3 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t)));
            uVar7 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t)));
            uVar8 = (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint64_t)));
            (*(uint64_t*)rtguest::G((uint32_t)puVar6)) = (*(uint64_t*)rtguest::G((uint32_t)param_3));
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t))) = uVar3;
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar7;
            (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t))) = uVar8;
            param_3 = param_3 + 4;
            puVar6 = puVar6 + 4;
          } while (param_3 != puVar1);
        }
      }
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
      uVar3 = CreateSema(auStack_b0);
      (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
      (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
      (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_90;
      sceSifWriteBackDCache(0x17788c0,0x420);
      lVar2 = sceSifCallRpc(0x1779b80,5,0,0x17788c0,0x420,0x1779500,4,0);
      iVar5 = (*(uint32_t*)rtguest::G(0x21779500));
      if (lVar2 < 0) {
        DeleteSema(uVar3);
        _sceFsSigSema();
        return 0xfffffff5;
      }
      _sceFsSigSema();
      if (iVar5 == 0) {
        DeleteSema(uVar3);
        return 0xfffffff5;
      }
      WaitSema(uVar3);
      DeleteSema(uVar3);

    }
    WaitSema((*(uint32_t*)rtguest::G(0x2e9994)));
    iVar5 = 0;
    if ((*(uint32_t*)rtguest::G(0x2e9908)) == -1) {
      piVar4 = 0x002e9908;
      iVar5 = 1;
      while ((piVar4 = piVar4 + 1, iVar5 < 0x20 && ((*(int32_t*)rtguest::G((uint32_t)piVar4)) == -1))) {
        iVar5 = iVar5 + 1;
      }
    }
    if (iVar5 == 0x20) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G(0x1778884)))) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G(0x1778884)))) = 1;
    }
    SignalSema((*(uint32_t*)rtguest::G(0x2e9994)));
  }
  else {
    if (param_2 != 3) {
      (*(uint32_t*)rtguest::G(0x17788cc)) = (*(uint32_t*)rtguest::G((uint32_t)puVar9));
      goto LAB_00289ba8;
    }
    (*(uint64_t*)rtguest::G((uint32_t)param_3)) = CONCAT44((*(uint32_t*)rtguest::G(0x21779554)),(*(uint32_t*)rtguest::G(0x21779550)));
  }
  _sceFsSigSema();
                    /* WARNING: Read-only address (ram,0x21779500) is written */
                    /* WARNING: Read-only address (ram,0x21779550) is written */
                    /* WARNING: Read-only address (ram,0x21779554) is written */
  return 0;
}


RT_GUEST_FN(0x00289a30, guest::sceIoctl)

}  // namespace guest
