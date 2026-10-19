// guest:0x0023c8a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_stamps_award_function(uint32_t param_1, ...) {
  byte bVar1;
  bool bVar2;
  uint uVar3;
uint32_t pbVar4;
  int iVar5;
uint32_t puVar6;
  undefined1 uVar7;
  
  if (param_1 == 0) {
    return 0;
  }
  if ((*(int32_t*)rtguest::G(0x1782a60)) == 8) {
    ::printf((char *)rtguest::G((uint32_t)(0x2ec880u)));
  }
  uVar3 = (int)(param_1 & 0xff) + 0xff;
  if ((param_1 & 0xff) == 0) {
    bVar2 = false;
    if ((*(uint64_t*)rtguest::G(0x177fc78)) == -1) {
      bVar2 = (*(uint32_t*)rtguest::G(0x177fc80)) == 0xfffffffff;
    }
  }
  else {
    bVar2 = ((*(uint64_t*)rtguest::G((uint32_t)0x177fc78 + (uint32_t)((uVar3 & 0xff) >> 6) * 8)) & 1L << ((long)(int)uVar3 & 0x3fU)) != 0;
  }
  if (bVar2) {
  }
  iVar5 = 0;
  if (0 < (*(int32_t*)rtguest::G(0x1782a60))) {
    pbVar4 = 0x01782a64;
    bVar1 = (*(uint32_t*)rtguest::G(0x1782a64));
    while( true ) {
      pbVar4 = pbVar4 + 1;
      if (bVar1 == param_1) {
      }
      iVar5 = iVar5 + 1;
      if ((*(int32_t*)rtguest::G(0x1782a60)) <= iVar5) break;
      bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar4));
    }
  }
  iVar5 = (*(int32_t*)rtguest::G(0x1782a60)) + 1;
  (*(uint32_t*)rtguest::G((uint32_t)0x1782a64 + (uint32_t)((*(int32_t*)rtguest::G(0x1782a60))) * 4)) = (char)param_1;
  (*(int32_t*)rtguest::G(0x1782a60)) = iVar5;
  if (param_1 == 0x1e) {
    uVar7 = 0x7d;
    if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
      puVar6 = 0x0177fd08;
      iVar5 = 0;
      do {
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar6)) = (*(uint8_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar6 = puVar6 + 1;
      } while (iVar5 < 0x7f);
      (*(uint32_t*)rtguest::G(0x177fd87)) = 0x7d;
      return 1;
    }
  }
  else if ((long)param_1 < 0x1f) {
    if (param_1 == 5) {
      uVar7 = 0x7b;
      if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
        puVar6 = 0x0177fd08;
        iVar5 = 0;
        do {
          iVar5 = iVar5 + 1;
          (*(uint8_t*)rtguest::G((uint32_t)puVar6)) = (*(uint8_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint8_t)));
          puVar6 = puVar6 + 1;
        } while (iVar5 < 0x7f);
        (*(uint32_t*)rtguest::G(0x177fd87)) = 0x7b;
      }
    }
    else {
      if (param_1 != 0x11) {
      }
      uVar7 = 0x7c;
      if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
        puVar6 = 0x0177fd08;
        iVar5 = 0;
        do {
          iVar5 = iVar5 + 1;
          (*(uint8_t*)rtguest::G((uint32_t)puVar6)) = (*(uint8_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint8_t)));
          puVar6 = puVar6 + 1;
        } while (iVar5 < 0x7f);
        (*(uint32_t*)rtguest::G(0x177fd87)) = 0x7c;
      }
    }
  }
  else if (param_1 == 0x32) {
    uVar7 = 0x1b;
    if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
      puVar6 = 0x0177fd08;
      iVar5 = 0;
      do {
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar6)) = (*(uint8_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar6 = puVar6 + 1;
      } while (iVar5 < 0x7f);
      (*(uint32_t*)rtguest::G(0x177fd87)) = 0x1b;
    }
  }
  else {
    if (param_1 != 100) {
    }
    uVar7 = 0x7f;
    if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
      puVar6 = 0x0177fd08;
      iVar5 = 0;
      do {
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar6)) = (*(uint8_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar6 = puVar6 + 1;
      } while (iVar5 < 0x7f);
      (*(uint32_t*)rtguest::G(0x177fd87)) = 0x7f;
    }
  }
  (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = uVar7;
  (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
}


RT_GUEST_FN(0x0023c8a8, guest::gs_stamps_award_function)

}  // namespace guest
