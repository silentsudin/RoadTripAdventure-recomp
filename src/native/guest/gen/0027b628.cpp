// guest:0x0027b628 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __do_global_dtors(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  int iVar1;
uint32_t puVar2;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G(0x002e89c0))));
  puVar2 = (uint32_t)(*(uint32_t*)rtguest::G(0x002e89c0));
  while (iVar1 != 0) {
    (*(uint32_t*)rtguest::G(0x002e89c0)) = (uint32_t)(puVar2 + 1);
    // MIPS 0x27b658: lw $a0,($v0); jalr $a0 — ctor slot holds the target directly; Ghidra's code** model double-derefs
    RT_CALLRA(0x0027b65c, rtCall((*(uint32_t*)rtguest::G((uint32_t)puVar2))));
    puVar2 = (uint32_t)(*(uint32_t*)rtguest::G(0x002e89c0));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(*(uint32_t*)rtguest::G(0x002e89c0))));
  }
  (*(uint32_t*)rtguest::G(0x002e89c0)) = (uint32_t)puVar2;
  if ((*(uint32_t*)rtguest::G(0x2e89c4)) == 0) {
    (*(uint32_t*)rtguest::G(0x2e89c4)) = 1;
    __deregister_frame_info(0x2e99c0);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0027b628, guest::__do_global_dtors)

}  // namespace guest
