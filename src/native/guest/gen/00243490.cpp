// guest:0x00243490 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#define GetTableEntry_ed720 GetTableEntry_ed720_decl
#include "guest_decls.inc"
#undef GetTableEntry_ed720

namespace guest {

uint32_t GetTableEntry_ed720(uint64_t param_1, uint32_t param_2, ...) {
  return (*(uint32_t*)rtguest::G((uint32_t)(0x2ed720u + (uint32_t)(param_2 * 0x11) * 4)));
}


RT_GUEST_FN(0x00243490, guest::GetTableEntry_ed720)

}  // namespace guest
