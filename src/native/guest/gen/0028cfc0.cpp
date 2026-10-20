// guest:0x0028cfc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t supplement_crt0(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x28) = 0x00000001u;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000001u;
    *(uint32_t*)rtguest::G(rt_sv+0x8) = 0x00000001u;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000001u;
  }
  // Hand-fixed per disasm 0x28cfc0-0x28d004 (field slots: Ghidra's pseudocode
  // misassigned the sw offsets; sd $2,0x28 = struct1.count+0x28 == struct2.init,
  // [delay sw v0,0x24] = sema1 id -> struct2.max). DAT pairing follows the
  // oracle's executed behavior (eerm_p8 probe: 0x2e99a8=sema1, 0x2e99ac=sema2).
  uint32_t f = rtguest::allocFrame(0x50u);
  (*(uint32_t*)rtguest::G(f + 0x28u)) = 1u;
  (*(uint32_t*)rtguest::G(f + 0x04u)) = 1u;
  (*(uint32_t*)rtguest::G(f + 0x08u)) = 1u;
  const int32_t sema1 = RT_CALLRA(0x28cfdc, guest::CreateSema(f));
  (*(uint32_t*)rtguest::G(f + 0x24u)) = uint32_t(sema1);
  const int32_t sema2 = RT_CALLRA(0x28cfec, guest::CreateSema(f + 0x20u));
  (*(uint32_t*)rtguest::G(0x2e99a8u)) = uint32_t(sema1);
  (*(uint32_t*)rtguest::G(0x2e99acu)) = uint32_t(sema2);
  return 0;
}


RT_GUEST_FN(0x0028cfc0, guest::supplement_crt0)

}  // namespace guest
