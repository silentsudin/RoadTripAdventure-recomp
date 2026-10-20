// guest:0x002237f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_00223790_selectCarAttachmentCase7(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(0);
  vu0ApplyMatrix();
  return 0;
}


RT_GUEST_FN(0x002237f8, guest::switchD_00223790_selectCarAttachmentCase7)

}  // namespace guest
