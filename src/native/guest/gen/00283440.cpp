// guest:0x00283440 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __smakebuf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x80, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x88) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x8c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t auStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
  bool bVar1;
  long lVar2;
  ushort uVar3;
  
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
  if ((uVar3 & 2) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(int32_t))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int32_t))) = (int)param_1 + 0x43;
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)param_1 + 0x43;
    return 0;
  }
  if (-1 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xe)))) {
    lVar2 = RT_CALLRA(0x283490, guest::_fstat_r((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x15) * sizeof(int32_t))),(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xe))),auStack_90));
    if (-1 < lVar2) {
      bVar1 = ((*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) & 0xf000) == 0x2000;
      if (((*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) & 0xf000) == 0x8000) {
        uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
        if ((uint32_t)(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(int32_t))) != 0x284838) goto LAB_002834f4;
        (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x13) * sizeof(int32_t))) = 0x400;
        uVar3 = uVar3 | 0x400;
      }
      else {
        uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
LAB_002834f4:
        uVar3 = uVar3 | 0x800;
      }
      (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = uVar3;
      goto LAB_002834fc;
    }
    uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
  }
  bVar1 = false;
  (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = uVar3 | 0x800;
LAB_002834fc:
  lVar2 = RT_CALLRA(0x283500, guest::_malloc_r((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x15) * sizeof(int32_t))),0x400));
  uVar3 = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t)));
  if (lVar2 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int32_t))) = (int)param_1 + 0x43;
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(int32_t))) = 1;
    (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = uVar3 | 2;
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)param_1 + 0x43;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(int32_t))) = (int)lVar2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x15) * sizeof(int32_t))) + 0x3c))) = 0x2829a8;
    (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = uVar3 | 0x80;
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(int32_t))) = 0x400;
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = (int)lVar2;
    if ((bVar1) && (lVar2 = sysIsatty((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xe)))), lVar2 != 0)) {
      (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) = (*(uint16_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint32_t))) | 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x00283440, guest::__smakebuf)

}  // namespace guest
