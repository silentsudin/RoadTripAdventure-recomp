// guest:0x002862c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyWrite(uint32_t param_1, uint32_t param_2, ...) {
  long lVar1;
  char cVar2;
uint32_t pcVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = 0;
  iVar5 = -1;
  if ((*(int32_t*)rtguest::G(0x1776a9c)) == 0) {
    DIntr();
    (*(int32_t*)rtguest::G(0x1776a9c)) = 1;
    (*(uint32_t*)rtguest::G(0x1776aa0)) = 0x21776ac0;
    pcVar3 = 0x21776acc;
    iVar5 = 0;
    do {
      param_2 = param_2 + -1;
      if (param_2 == -1) break;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == '\n') {
        (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = '\r';
        iVar4 = iVar4 + 1;
        pcVar3 = pcVar3 + 1;
        if (0xff < iVar4) break;
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      }
      (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = cVar2;
      iVar4 = iVar4 + 1;
      param_1 = param_1 + 1;
      pcVar3 = pcVar3 + 1;
      iVar5 = iVar5 + 1;
    } while (iVar4 < 0x100);
    (*(uint32_t*)rtguest::G(0x1776a94)) = iVar4 + 0xc;
    (*(uint32_t*)rtguest::G(0x21776ac0)) = (uint32_t)(*(uint32_t*)rtguest::G(0x1776a94));
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
    lVar1 = sceDeci2ReqSend((*(int32_t*)rtguest::G(0x1776a90)),(*(uint32_t*)rtguest::G(0x21776ac7)));
    if (lVar1 < 0) {
      (*(int32_t*)rtguest::G(0x1776a9c)) = 0;
      EIntr();
      iVar5 = -1;
    }
    else {
      while ((*(int32_t*)rtguest::G(0x1776a9c)) != 0) {
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
        sceDeci2Poll((*(int32_t*)rtguest::G(0x1776a90)));
      }
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
      EIntr();
    }
  }
  return iVar5;
}


RT_GUEST_FN(0x002862c8, guest::sceTtyWrite)

}  // namespace guest
