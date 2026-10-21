// guest:0x00282d70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: undefined4 _malloc_trim_r(undefined8, ulong)
uint32_t _malloc_trim_r(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  uint uVar1;
uint32_t puVar2;
  int iVar3;
  long lVar4;
  long lVar5;
  
  RT_CALLRA(0x282d98, guest::__malloc_lock());
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) & 0xfffffffc;
  lVar5 = (((uint32_t)uVar1 - (param_2 & 0xffffffff)) + 0xfef >> 0xc) - 1;
  if ((0xfff < lVar5 * 0x1000) &&
     (puVar2 = (uint32_t)_sbrk_r(param_1,0), puVar2 == (*(uint32_t*)rtguest::G(0x002e8cd0)) + uVar1)) {
    iVar3 = (int)((uint64_t)(lVar5 << 0x2c) >> 0x20);  // pseudocode: (ulong)(lVar5 << 0x2c) >> 0x20
    lVar4 = _sbrk_r(param_1,-iVar3);
    if (lVar4 != -1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) = uVar1 + (int)lVar5 * -0x1000 | 1;
      (*(uint32_t*)rtguest::G(0x2e90f8)) = (*(uint32_t*)rtguest::G(0x2e90f8)) - iVar3;
      RT_CALLRA(0x282e78, guest::__malloc_unlock(param_1));
      return 1;
    }
    iVar3 = _sbrk_r(param_1,0);
    if (0xf < iVar3 - (int)(*(uint32_t*)rtguest::G(0x002e8cd0))) {
      (*(uint32_t*)rtguest::G(0x2e90f8)) = iVar3 - (*(uint32_t*)rtguest::G(0x2e90e0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cd0)) + 4))) = iVar3 - (int)(*(uint32_t*)rtguest::G(0x002e8cd0)) | 1;
    }
  }
  RT_CALLRA(0x282eb0, guest::__malloc_unlock(param_1));
  return 0;
}


RT_GUEST_FN(0x00282d70, guest::_malloc_trim_r)

}  // namespace guest
