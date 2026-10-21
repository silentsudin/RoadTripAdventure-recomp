// guest:0x00288368 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifGetNextRequest(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  undefined4 uVar2;
  
  RT_CALLRA(0x288378, guest::DIntr());
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc)));
  if (iVar1 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 0;
  }
  else {
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 4))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0xc))) = uVar2;
  }
  RT_CALLRA(0x2883a0, guest::EIntr());
  return iVar1;
}


RT_GUEST_FN(0x00288368, guest::sceSifGetNextRequest)

}  // namespace guest
