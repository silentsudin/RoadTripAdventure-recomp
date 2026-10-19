// guest:0x0027dc30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t free(uint32_t __ptr, ...) {
  __malloc_lock((*(uint32_t*)rtguest::G(0x002e8cc0)));
  _free_r((*(uint32_t*)rtguest::G(0x002e8cc0)),__ptr);
  __malloc_unlock((*(uint32_t*)rtguest::G(0x002e8cc0)));
  return 0;
}


RT_GUEST_FN(0x0027dc30, guest::free)

}  // namespace guest
