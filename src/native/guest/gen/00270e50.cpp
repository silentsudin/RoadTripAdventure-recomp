// guest:0x00270e50 transliterated from Ghidra pseudocode
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridDestroy(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  heapFreeBlock((*(uint32_t*)rtguest::G(param_1 + 2 * sizeof(uint32_t))));
  (*(uint32_t*)rtguest::G(param_1 + 1 * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G(param_1 + 2 * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G(param_1)) = 0;
  return 0;
}

RT_GUEST_FN(0x00270e50, guest::textGridDestroy)

}  // namespace guest
