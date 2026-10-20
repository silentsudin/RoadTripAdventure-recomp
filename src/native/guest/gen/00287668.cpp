// guest:0x00287668 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInitRpc(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  long lVar1;
  
  RT_CALLRA(0x287678, guest::DIntr());
  if ((*(int32_t*)rtguest::G(0x2e9900)) != 0) {
    EIntr();
    return 0;
  }
  (*(int32_t*)rtguest::G(0x2e9900)) = 1;
  EIntr();
  RT_CALLRA(0x2876b0, guest::sceSifInitCmd());
  RT_CALLRA(0x2876b8, guest::DIntr());
  (*(uint32_t*)rtguest::G(0x1778860)) = 0x20;
  (*(uint32_t*)rtguest::G(0x1778840)) = 1;
  (*(uint32_t*)rtguest::G(0x1778854)) = 0x21777840;
  (*(uint32_t*)rtguest::G(0x1778844)) = 0x21777040;
  (*(uint32_t*)rtguest::G(0x177885c)) = 0x21778040;
  (*(uint32_t*)rtguest::G(0x1778848)) = 0x20;
  (*(uint32_t*)rtguest::G(0x177884c)) = 0;
  (*(uint32_t*)rtguest::G(0x1778850)) = 0;
  (*(uint32_t*)rtguest::G(0x1778858)) = 0x20;
  (*(uint32_t*)rtguest::G(0x1778864)) = 0;
  RT_CALLRA(0x28772c, guest::sceSifAddCmdHandler(0xffffffff80000008,0x287968,0x1778840));
  RT_CALLRA(0x287744, guest::sceSifAddCmdHandler(0xffffffff80000009,0x287c28,0x1778840));
  RT_CALLRA(0x28775c, guest::sceSifAddCmdHandler(0xffffffff8000000a,0x287e18,0x1778840));
  RT_CALLRA(0x287774, guest::sceSifAddCmdHandler(0xffffffff8000000c,0x287a20,0x1778840));
  EIntr();
  lVar1 = RT_CALLRA(0x287788, guest::sceSifGetReg(0xffffffff80000002));
  if (lVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x177708c)) = 1;
    RT_CALLRA(0x2877b4, guest::sceSifSendCmd(0xffffffff80000002,0x1777080,0x10,0,0,0));
    do {
      lVar1 = RT_CALLRA(0x2877c0, guest::sifCmdGetFlag(0));
    } while (lVar1 == 0);
    sceSifSetReg(0xffffffff80000002,1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00287668, guest::sceSifInitRpc)

}  // namespace guest
