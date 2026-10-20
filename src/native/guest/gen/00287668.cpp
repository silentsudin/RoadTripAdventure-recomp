// guest:0x00287668 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInitRpc(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  long lVar1;
  
  DIntr();
  if ((*(int32_t*)rtguest::G(0x2e9900)) != 0) {
    EIntr();
    return 0;
  }
  (*(int32_t*)rtguest::G(0x2e9900)) = 1;
  EIntr();
  sceSifInitCmd();
  DIntr();
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
  sceSifAddCmdHandler(0xffffffff80000008,0x287968,0x1778840);
  sceSifAddCmdHandler(0xffffffff80000009,0x287c28,0x1778840);
  sceSifAddCmdHandler(0xffffffff8000000a,0x287e18,0x1778840);
  sceSifAddCmdHandler(0xffffffff8000000c,0x287a20,0x1778840);
  EIntr();
  lVar1 = sceSifGetReg(0xffffffff80000002);
  if (lVar1 == 0) {
    (*(uint32_t*)rtguest::G(0x177708c)) = 1;
    sceSifSendCmd(0xffffffff80000002,0x1777080,0x10,0,0,0);
    do {
      lVar1 = sifCmdGetFlag(0);
    } while (lVar1 == 0);
    sceSifSetReg(0xffffffff80000002,1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00287668, guest::sceSifInitRpc)

}  // namespace guest
