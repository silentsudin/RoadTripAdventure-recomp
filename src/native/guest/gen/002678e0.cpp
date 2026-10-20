// guest:0x002678e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t KnockCollectMaskGates(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  long lVar1;
  
  lVar1 = TestStateBitFc78(0x5c);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x20;
  }
  lVar1 = TestStateBitFc78(0x27);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x40;
  }
  lVar1 = TestStateBitFc78(0x61);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x80;
  }
  lVar1 = TestStateBitFc78(0x13);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x100;
  }
  lVar1 = TestStateBitFc78(0x43);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x200;
  }
  lVar1 = TestStateBitFc78(0x59);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x400;
  }
  lVar1 = TestStateBitFc78(0x3b);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x2000;
  }
  return 0;
}


RT_GUEST_FN(0x002678e0, guest::KnockCollectMaskGates)

}  // namespace guest
