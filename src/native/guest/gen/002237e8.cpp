// guest:0x002237e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_00223790_selectCarAttachmentCase8(uint32_t q0, ...) {
  int in_a3_lo;
  undefined4 uStack00000004;
  undefined4 uStack00000008;
  
  uStack00000008 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(in_a3_lo + 0x34)));
  uStack00000004 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(in_a3_lo + 0x30)));
  vu0ApplyMatrix();
  return 0;
}


RT_GUEST_FN(0x002237e8, guest::switchD_00223790_selectCarAttachmentCase8)

}  // namespace guest
