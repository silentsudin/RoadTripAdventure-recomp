// guest:0x00237ac0 transliterated from Ghidra pseudocode + MIPS 0x237ac0..0x237c43 — DO NOT hand-edit
// MIPS corrections to pseudocode: the "ANYBODY" assignments are u64 loads of the 8 string bytes
// at 0x333870 (ld/sd, not pointer stores); part-2 table bases are direct (0x1783220/0x1783228),
// not through DAT_01783220; part-2 pid is DAT_01782b98; the char[8] return is fiction — v0's
// low word survives (matched id / strcpy dst / ANYBODY word) and that is what callers see.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LookupPlayerNameSlot(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::gS[2] = 0x0000000001780000ull;  // walk const
  }
  bool bVar1;
  uint64_t idw;
  uint32_t namep;
  uint32_t idp;
  int iVar8;
  uint32_t v0;

  v0 = 0;
  iVar8 = 0;
  if ((*(uint8_t*)rtguest::G(0x0177fde0)) == 0) {
    bVar1 = true;
LAB_00237b3c:
    if (bVar1) {
      (*(uint64_t*)rtguest::G(0x0177fdd8 + (uint32_t)iVar8 * 0x30)) = (*(uint64_t*)rtguest::G(0x01785fe0));
      idp = 0x0177fde0 + (uint32_t)iVar8 * 0x30;
      v0 = idp;  // guest strcpy thunk (0x27f1e0) returns dst
      ::strcpy((char*)rtguest::G((uint32_t)idp), (char*)rtguest::G(0x017831d4));
      (*(uint16_t*)rtguest::G((uint32_t)q0 + 0x1c)) = (uint16_t)iVar8;
      goto LAB_00237b84;
    }
LAB_00237b70:
    iVar8 = 0x13;
    v0 = (*(uint32_t*)rtguest::G(0x00333870));
    (*(uint64_t*)rtguest::G(0x01780170)) = (*(uint64_t*)rtguest::G(0x00333870));
  }
  else {
    namep = 0x0177fde0;
    idp = 0x0177fdd8;
    idw = (*(uint64_t*)rtguest::G((uint32_t)idp));
    while (idp = idp + 0x30, idw != (*(uint64_t*)rtguest::G(0x01785fe0))) {
      iVar8 = iVar8 + 1;
      bVar1 = iVar8 < 0x14;
      namep = namep + 0x30;
      if (!bVar1) goto LAB_00237b70;
      if ((*(uint8_t*)rtguest::G((uint32_t)namep)) == 0) goto LAB_00237b3c;
      idw = (*(uint64_t*)rtguest::G((uint32_t)idp));
    }
    v0 = (uint32_t)idw;
  }
  (*(uint16_t*)rtguest::G((uint32_t)q0 + 0x1c)) = (uint16_t)iVar8;
LAB_00237b84:
  iVar8 = 0;
  if ((*(uint8_t*)rtguest::G(0x01783228)) == 0) {
    bVar1 = true;
LAB_00237be4:
    if (bVar1) {
      (*(uint64_t*)rtguest::G(0x01783220 + (uint32_t)iVar8 * 0x30)) = (*(uint64_t*)rtguest::G(0x01782b98));
      idp = 0x01783228 + (uint32_t)iVar8 * 0x30;
      v0 = idp;
      ::strcpy((char*)rtguest::G((uint32_t)idp), (char*)rtguest::G(0x0177fd8c));
      (*(uint16_t*)rtguest::G((uint32_t)q0 + 0x1e)) = (uint16_t)iVar8;
      goto LAB_00237c2c;
    }
LAB_00237c18:
    iVar8 = 0x13;
    v0 = (*(uint32_t*)rtguest::G(0x00333870));
    (*(uint64_t*)rtguest::G(0x017835b8)) = (*(uint64_t*)rtguest::G(0x00333870));
  }
  else {
    namep = 0x01783228;
    idp = 0x01783220;
    idw = (*(uint64_t*)rtguest::G((uint32_t)idp));
    while (idp = idp + 0x30, idw != (*(uint64_t*)rtguest::G(0x01782b98))) {
      iVar8 = iVar8 + 1;
      bVar1 = iVar8 < 0x14;
      namep = namep + 0x30;
      if (!bVar1) goto LAB_00237c18;
      if ((*(uint8_t*)rtguest::G((uint32_t)namep)) == 0) goto LAB_00237be4;
      idw = (*(uint64_t*)rtguest::G((uint32_t)idp));
    }
    v0 = (uint32_t)idw;
  }
  (*(uint16_t*)rtguest::G((uint32_t)q0 + 0x1e)) = (uint16_t)iVar8;
LAB_00237c2c:
  return v0;
}


RT_GUEST_FN(0x00237ac0, guest::LookupPlayerNameSlot)

}  // namespace guest
