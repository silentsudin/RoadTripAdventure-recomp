// guest:0x00267a80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HintCollectMaskGates(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x267a90, guest::TestStateBitFc78(0x2a));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x20000000;
  }
  lVar1 = RT_CALLRA(0x267aac, guest::TestStateBitFc78(0x54));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x40000000;
  }
  lVar1 = RT_CALLRA(0x267ac8, guest::TestStateBitFc78(0x41));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) | 2;
  }
  lVar1 = RT_CALLRA(0x267ae4, guest::TestStateBitFc78(0x3f));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) | 0x10;
  }
  lVar1 = RT_CALLRA(0x267b00, guest::TestFlagBitFc98(0x5d));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x8000;
  }
  lVar1 = RT_CALLRA(0x267b1c, guest::TestFlagBitFc98(0x5e));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x10000;
  }
  lVar1 = RT_CALLRA(0x267b3c, guest::TestStateBitFc78(0x59));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x40000;
  }
  lVar1 = RT_CALLRA(0x267b5c, guest::TestFlagBitFc98(0x5f));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x80000;
  }
  lVar1 = RT_CALLRA(0x267b7c, guest::TestStateBitFc78(0x2c));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x100000;
  }
  lVar1 = RT_CALLRA(0x267b9c, guest::TestFlagBitFc98(0x5c));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x200000;
  }
  lVar1 = RT_CALLRA(0x267bbc, guest::TestStateBitFc78(9));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x400000;
  }
  lVar1 = RT_CALLRA(0x267bdc, guest::TestStateBitFc78(0x30));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) | 0x1000000;
  }
  lVar1 = RT_CALLRA(0x267bfc, guest::TestStateBitFc78(100));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) | 0x100000;
  }
  return 0;
}


RT_GUEST_FN(0x00267a80, guest::HintCollectMaskGates)

}  // namespace guest
