// guest:0x0027dbd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t malloc(uint32_t __size, ...) {
uint32_t pvVar1;
  
  __malloc_lock((*(uint32_t*)rtguest::G(0x002e8cc0)));
  pvVar1 = (uint32_t)_malloc_r((*(uint32_t*)rtguest::G(0x002e8cc0)),__size);
  __malloc_unlock((*(uint32_t*)rtguest::G(0x002e8cc0)));
  return pvVar1;
}


RT_GUEST_FN(0x0027dbd8, guest::malloc)

}  // namespace guest
