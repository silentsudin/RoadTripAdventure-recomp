// guest:0x00267a80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HintCollectMaskGates(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  long lVar1;
  
  lVar1 = TestStateBitFc78(0x2a);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x20000000;
  }
  lVar1 = TestStateBitFc78(0x54);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x40000000;
  }
  lVar1 = TestStateBitFc78(0x41);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) | 2;
  }
  lVar1 = TestStateBitFc78(0x3f);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) | 0x10;
  }
  lVar1 = TestFlagBitFc98(0x5d);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x8000;
  }
  lVar1 = TestFlagBitFc98(0x5e);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x10000;
  }
  lVar1 = TestStateBitFc78(0x59);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x40000;
  }
  lVar1 = TestFlagBitFc98(0x5f);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x80000;
  }
  lVar1 = TestStateBitFc78(0x2c);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x100000;
  }
  lVar1 = TestFlagBitFc98(0x5c);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x200000;
  }
  lVar1 = TestStateBitFc78(9);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x400000;
  }
  lVar1 = TestStateBitFc78(0x30);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x1000000;
  }
  lVar1 = TestStateBitFc78(100);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) | 0x100000;
  }
  return 0;
}


RT_GUEST_FN(0x00267a80, guest::HintCollectMaskGates)

}  // namespace guest
