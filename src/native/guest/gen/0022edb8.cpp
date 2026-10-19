// guest:0x0022edb8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawDualScoreNumbers(uint32_t q0, ...) {
  DrawDialogTextLine(0x140,0x2a,(*(uint32_t*)rtguest::G(0x177fdb4)),0x177fd9f);
  DrawDialogTextLine(0x1b8,0x4a,(*(uint32_t*)rtguest::G(0x17831fc)),0x17831e7);
  return 0;
}


RT_GUEST_FN(0x0022edb8, guest::DrawDualScoreNumbers)

}  // namespace guest
