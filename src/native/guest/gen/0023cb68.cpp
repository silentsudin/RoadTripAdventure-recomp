// guest:0x0023cb68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetProgressFlagAndQueue(uint32_t param_1, ...) {
  char cVar1;
  long lVar2;
  uint uVar3;
uint32_t puVar4;
  int iVar5;
  
  uVar3 = 1 << ((uint32_t)param_1 & 0x1f);
  if (((*(uint32_t*)rtguest::G(0x177fdd0)) & uVar3) == 0) {
    (*(uint32_t*)rtguest::G(0x177fdd0)) = (*(uint32_t*)rtguest::G(0x177fdd0)) | uVar3;
    if ((*(uint32_t*)rtguest::G((uint32_t)0x2a2b38 + (uint32_t)((uint32_t)param_1) * 4)) != -1) {
      cVar1 = (*(uint32_t*)rtguest::G((uint32_t)0x2a2b38 + (uint32_t)((uint32_t)param_1) * 4));
      if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
        puVar4 = 0x0177fd08;
        iVar5 = 0;
        do {
          iVar5 = iVar5 + 1;
          (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint8_t)));
          puVar4 = puVar4 + 1;
          (*(uint32_t*)rtguest::G(0x177fd87)) = cVar1;
        } while (iVar5 < 0x7f);
      }
      else {
        (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = cVar1;
        (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
      }
      if (param_1 == 0) {
        (*(uint64_t*)rtguest::G(0x177fc88)) = (*(uint64_t*)rtguest::G(0x177fc88)) | 0x1000000000000;
      }
    }
  }
  lVar2 = CountSetBits((*(uint32_t*)rtguest::G(0x177fdd0)));
  if (lVar2 == 0xd) {
    (*(uint32_t*)rtguest::G(0x177fdd0)) = (*(uint32_t*)rtguest::G(0x177fdd0)) | 0x200;
  }
  if ((*(uint32_t*)rtguest::G(0x177fdd0)) == 0x3ffffef) {
    (*(uint32_t*)rtguest::G(0x177fdd0)) = 0x3ffffff;
    gs_stamps_award_function(0x33);
    if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
      puVar4 = 0x0177fd08;
      iVar5 = 0;
      do {
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar4 = puVar4 + 1;
      } while (iVar5 < 0x7f);
      (*(uint32_t*)rtguest::G(0x177fd87)) = '\x1c';
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = 0x1c;
      (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
    }
    if ((*(int32_t*)rtguest::G(0x177fd88)) == 0x80) {
      puVar4 = 0x0177fd08;
      iVar5 = 0;
      do {
        iVar5 = iVar5 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint8_t)));
        puVar4 = puVar4 + 1;
      } while (iVar5 < 0x7f);
      (*(uint32_t*)rtguest::G(0x177fd87)) = '\x1d';
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)0x177fd08 + (uint32_t)((*(int32_t*)rtguest::G(0x177fd88))) * 1)) = 0x1d;
      (*(int32_t*)rtguest::G(0x177fd88)) = (*(int32_t*)rtguest::G(0x177fd88)) + 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0023cb68, guest::SetProgressFlagAndQueue)

}  // namespace guest
