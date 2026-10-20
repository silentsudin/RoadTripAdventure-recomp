// guest:0x002679c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t MailCollectMaskGates(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x2679d8, guest::TestStateBitFc78(0x5a));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 4;
  }
  lVar1 = RT_CALLRA(0x2679f4, guest::TestStateBitFc78(0x2c));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 8;
  }
  lVar1 = RT_CALLRA(0x267a10, guest::TestStateBitFc78(0x11));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x20;
  }
  RT_CALLRA(0x267a2c, guest::TestStateBitFc78(0x2f));
  lVar1 = RT_CALLRA(0x267a34, guest::TestStateBitFc78(0xe));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x80;
  }
  lVar1 = RT_CALLRA(0x267a50, guest::TestStateBitFc78(0x59));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = (*(uint32_t*)rtguest::G((uint32_t)param_1)) | 0x100;
  }
  return 0;
}


RT_GUEST_FN(0x002679c8, guest::MailCollectMaskGates)

}  // namespace guest
