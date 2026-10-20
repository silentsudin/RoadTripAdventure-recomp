// guest:0x002423b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitializeEffectPackets(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  (*(uint64_t*)rtguest::G(0x177ea48)) = 0x20058245dd406bf2;
  (*(uint64_t*)rtguest::G(0x177ea30)) = 0x20056cc5e140ab26;
  (*(uint64_t*)rtguest::G(0x177ea38)) = 0x20057545e140ab6a;
  (*(uint64_t*)rtguest::G(0x177ea40)) = 0x200579c5dd406bae;
  return 0;
}


RT_GUEST_FN(0x002423b8, guest::InitializeEffectPackets)

}  // namespace guest
