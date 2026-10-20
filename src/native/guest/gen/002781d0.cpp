// guest:0x002781d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PowerOffCB(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  (*(uint32_t*)rtguest::G(0x2e6f24)) = 1;
  RT_CALLRA(0x2781e8, guest::DIntr());
  RT_CALLRA(0x278200, guest::sceSifAddCmdHandler(0xffffffff80000012,0x278190,0));
  RT_CALLRA(0x278208, guest::EIntr());
  (*(uint32_t*)rtguest::G(0x2e6f24)) = 0;
  (*(int32_t*)rtguest::G(0x2e6f3c)) = 1;
  return 1;
}


RT_GUEST_FN(0x002781d0, guest::PowerOffCB)

}  // namespace guest
