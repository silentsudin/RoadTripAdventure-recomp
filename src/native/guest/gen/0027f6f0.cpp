// guest:0x0027f6f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vfprintf(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
  }
uint32_t puVar1;
uint32_t puVar2;
  int iVar3;
  
  puVar1 = (*(uint32_t*)rtguest::G(0x002e8cc0));
  iVar3 = (int)param_1;
  puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x54)));
  if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x54))) == (uint32_t)0x0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x54))) = (*(uint32_t*)rtguest::G(0x002e8cc0));
    puVar2 = puVar1;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x38))) == 0) {
    RT_CALLRA(0x27f734, guest::__sinit(puVar2,param_1));
    puVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x54)));
  }
  _vfprintf_r(puVar2,param_1,param_2,param_3);
  return 0;
}


RT_GUEST_FN(0x0027f6f0, guest::vfprintf)

}  // namespace guest
