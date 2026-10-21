// guest:0x00278008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cmd_sem_init(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x30, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::gS[1] = 0x00000000002e0000ull;  // walk const
  }
    // sema struct fields live at guest offsets inside one contiguous block
    uint32_t auStack_50 = rtguest::allocFrame(0x18);
  
  if (((*(int32_t*)rtguest::G(0x2e6f28)) == -1) || ((*(int32_t*)rtguest::G(0x2e6f2c)) == -1)) {
    (*(uint32_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(0x14))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(8))) = 1;
    (*(int32_t*)rtguest::G(0x2e6f28)) = RT_CALLRA(0x278054, guest::CreateSema(auStack_50));
    (*(int32_t*)rtguest::G(0x2e6f2c)) = RT_CALLRA(0x278060, guest::CreateSema(auStack_50));
    (*(uint32_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(8))) = 0;
    (*(uint32_t*)rtguest::G(0x2e6f20)) = RT_CALLRA(0x278070, guest::CreateSema(auStack_50));
    (*(uint32_t*)rtguest::G(0x2e6f30)) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00278008, guest::cmd_sem_init)

}  // namespace guest
