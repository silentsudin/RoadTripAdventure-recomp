// guest:0x0023c5c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_photos_take_function(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  ulong uVar3;
  int iVar4;
  
  uVar1 = (*(uint32_t*)rtguest::G(0x177f768));
  if (param_1 != 0) {
    iVar4 = (int)param_1 + -1;
    iVar2 = iVar4;
    if ((long)iVar4 < 0) {
      iVar2 = (int)param_1 + 0x3e;
    }
    uVar3 = (*(uint64_t*)rtguest::G((uint32_t)0x177fc68 + (uint32_t)(iVar2 >> 6) * 8));
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f7b4 + iVar4 * 0xc))) = (*(uint64_t*)rtguest::G(0x177f760));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f7bc + iVar4 * 0xc))) = uVar1;
    (*(uint64_t*)rtguest::G((uint32_t)0x177fc68 + (uint32_t)(iVar2 >> 6) * 8)) = uVar3 | 1L << ((long)iVar4 & 0x3fU);
    (*(uint16_t*)rtguest::G(0x1780a42)) = (uint32_t)iVar4;
    if ((*(uint64_t*)rtguest::G(0x177fc68)) == -1 && (*(uint64_t*)rtguest::G(0x177fc70)) == 0xfffffffff) {
      RT_CALLRA(0x23c678, guest::gs_stamps_award_function((uint64_t)0x60));
    }
    (*(uint64_t*)rtguest::G(0x177fc90)) = (*(uint64_t*)rtguest::G(0x177fc90)) | 0x80000000;
  }
  return 0;
}


RT_GUEST_FN(0x0023c5c0, guest::gs_photos_take_function)

}  // namespace guest
