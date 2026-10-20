// guest:0x002864e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyInit(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  
  FlushCache(0);
  (*(int32_t*)rtguest::G(0x1776a90)) = sceDeci2Open(0x210,0x1776a90,0x286130);
  bVar1 = -1 < (*(int32_t*)rtguest::G(0x1776a90));
  if (bVar1) {
    (*(int32_t*)rtguest::G(0x1776a9c)) = 0;
    (*(uint32_t*)rtguest::G(0x1776a94)) = 0;
    (*(uint32_t*)rtguest::G(0x1776a98)) = 0;
    (*(uint32_t*)rtguest::G(0x1776aa4)) = 0x21776c00;
    (*(uint32_t*)rtguest::G(0x1776aa0)) = 0x21776ac0;
    (*(uint32_t*)rtguest::G(0x21776ac4)) = 0x210;
    (*(uint32_t*)rtguest::G(0x21776ac6)) = 0x45;
    (*(uint32_t*)rtguest::G(0x21776ac7)) = 0x48;
    (*(uint32_t*)rtguest::G(0x21776ac8)) = 0;
    (*(uint32_t*)rtguest::G(0x21776ac2)) = 0;
    (*(int32_t*)rtguest::G(0x1776aa8)) = ttyRingInit(0x100);
  }
  return bVar1;
}


RT_GUEST_FN(0x002864e8, guest::sceTtyInit)

}  // namespace guest
