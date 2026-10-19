// guest:0x0028a2f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceFormat(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
uint32_t auStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(16);
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  long lVar4;
uint32_t puVar5;
  int iVar6;
  
  _sceFsWaitS(0xe);
  if ((*(int32_t*)rtguest::G(0x2e9988)) == 0) {
    sceFsInit();
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
  if (param_2 == 0) {
    (*(uint32_t*)rtguest::G(0x1778ccc)) = '\0';
  }
  else {
    (*(uint32_t*)rtguest::G(0x1778ccc)) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    iVar6 = 0;
    if ((*(uint32_t*)rtguest::G(0x1778ccc)) != '\0') {
      for (iVar6 = 1;
          (iVar6 < 0x400 &&
          (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(iVar6) * 1)), (*(uint32_t*)rtguest::G((uint32_t)0x1778ccc + (uint32_t)(iVar6) * 4)) = cVar1, cVar1 != '\0'));
          iVar6 = iVar6 + 1) {
      }
    }
    if (iVar6 == 0x400) {
      (*(uint32_t*)rtguest::G(0x17790cb)) = 0;
    }
  }
  if (param_4 < 0x401) {
    iVar6 = 0;
    if (0 < param_4) {
      do {
        puVar2 = (uint32_t)(param_3 + iVar6);
        puVar5 = (uint32_t)((int)0x017790cc + iVar6);
        iVar6 = iVar6 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar5)) = (*(uint8_t*)rtguest::G((uint32_t)puVar2));
      } while (iVar6 < param_4);
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
    (*(uint32_t*)rtguest::G(0x17794cc)) = param_4;
    uVar3 = CreateSema(auStack_c0);
    (*(uint32_t*)rtguest::G(0x17788c8)) = 4;
    (*(uint32_t*)rtguest::G(0x17788c0)) = (uint32_t)uVar3;
    (*(uint32_t*)rtguest::G(0x17788c4)) = auStack_a0;
    sceSifWriteBackDCache(0x17788c0,0xc10);
    lVar4 = sceSifCallRpc(0x1779b80,0xe,0,0x17788c0,0xc10,0x1779500,4,0);
    iVar6 = (*(uint32_t*)rtguest::G(0x21779500));
    if (lVar4 < 0) {
      DeleteSema(uVar3);
      _sceFsSigSema();
      (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
    }
    else {
      _sceFsSigSema();
      if (iVar6 == 0) {
        DeleteSema(uVar3);
        (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff5;
      }
      else {
        WaitSema(uVar3);
        DeleteSema(uVar3);
      }
    }
  }
  else {
    _sceFsSigSema();
    (*(uint32_t*)rtguest::G((uint32_t)auStack_a0 + (uint32_t)(0) * 4)) = 0xfffffff9;
  }

}


RT_GUEST_FN(0x0028a2f0, guest::sceFormat)

}  // namespace guest
