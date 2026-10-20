// guest:0x002679c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MailCollectMaskGates(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  long lVar1;
  
  lVar1 = TestStateBitFc78(0x5a);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 4;
  }
  lVar1 = TestStateBitFc78(0x2c);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 8;
  }
  lVar1 = TestStateBitFc78(0x11);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x20;
  }
  TestStateBitFc78(0x2f);
  lVar1 = TestStateBitFc78(0xe);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x80;
  }
  lVar1 = TestStateBitFc78(0x59);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x100;
  }
  return 0;
}


RT_GUEST_FN(0x002679c8, guest::MailCollectMaskGates)

}  // namespace guest
