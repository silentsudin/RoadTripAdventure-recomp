// guest:0x00286f40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifCmd0StoreWord(uint32_t param_1, uint32_t param_2, ...) {
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  return 0;
}


RT_GUEST_FN(0x00286f40, guest::sifCmd0StoreWord)

}  // namespace guest
