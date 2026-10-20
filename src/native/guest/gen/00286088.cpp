// guest:0x00286088 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ttyRingInit(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint32_t*)rtguest::G(0x1776980)) = param_1;
  (*(uint32_t*)rtguest::G(0x1776988)) = 0x01776990;
  (*(uint32_t*)rtguest::G(0x1776984)) = 0;
  (*(uint32_t*)rtguest::G(0x177698c)) = 0x01776990;
  return 0x01776980;
}


RT_GUEST_FN(0x00286088, guest::ttyRingInit)

}  // namespace guest
