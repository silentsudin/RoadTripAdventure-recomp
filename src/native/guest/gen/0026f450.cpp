// guest:0x0026f450 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefDBuffDc(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg(208);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xc0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc4) = 0x00000000u;
  }
uint32_t puVar1;
  ulong uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  puVar1 = (uint32_t)RT_CALLRA(0x26f4a4, guest::sceGsGetEnv());
  iVar6 = (param_3 << 0x10) >> 0x10;
  iVar7 = (param_4 << 0x10) >> 0x10;
  RT_CALLRA(0x26f4cc, guest::sceGsSetDefDispEnv(param_1,param_2,iVar6,iVar7,0,0));
  iVar5 = (int)param_1;
  RT_CALLRA(0x26f4e8, guest::sceGsSetDefDispEnv(iVar5 + 0x28,param_2,iVar6,iVar7,0,0));
  RT_CALLRA(0x26f504, guest::sceGsSetDefDrawEnv(iVar5 + 0x60,param_2,iVar6,iVar7,param_5,param_6));
  RT_CALLRA(0x26f520, guest::sceGsSetDefDrawEnv2(iVar5 + 0xe0,param_2,iVar6,iVar7,param_5,param_6));
  RT_CALLRA(0x26f53c, guest::sceGsSetDefDrawEnv(iVar5 + 0x1d0,param_2,iVar6,iVar7,param_5,param_6));
  RT_CALLRA(0x26f558, guest::sceGsSetDefDrawEnv2(iVar5 + 0x250,param_2,iVar6,iVar7,param_5,param_6));
  if (param_7 != 0) {
    iVar4 = 0x800 - ((param_4 << 0x10) >> 0x11);
    iVar3 = 0x800 - ((param_3 << 0x10) >> 0x11);
    RT_CALLRA(0x26f5a0, guest::sceGsSetDefClear(iVar5 + 0x160,param_5,iVar3,iVar4,iVar6,iVar7,0,0));
    RT_CALLRA(0x26f5d0, guest::sceGsSetDefClear(iVar5 + 0x2d0,param_5,iVar3,iVar4,iVar6,iVar7,0,0));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x54))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x58))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x5c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c0))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c4))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1cc))) = 0;
  uVar2 = 0x10;
  if (param_7 != 0) {
    uVar2 = 0x16;
  }
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) =
       (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) & 0xfffffffffff8000 | uVar2 | 0x1000000000008000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x58))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x58))) & 0xfffffffffffffff0 | 0xe;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c0))) =
       (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c0))) & 0xfffffffffff8000 | uVar2 | 0x1000000000008000;
  (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c8))) & 0xfffffffffffffff0 | 0xe;
  iVar6 = RT_CALLRA(0x26f678, guest::sceGszbufaddr(param_2,iVar6,iVar7));
  if ((((*(uint64_t*)rtguest::G((uint32_t)puVar1)) & 0xffff0000ffff) == 0x100000001) || ((short)(*(uint64_t*)rtguest::G((uint32_t)puVar1)) == 0)) {
    uVar2 = (long)(iVar6 >> 1) & 0x1ff;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe0))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe0))) & 0xfffffffffffffe00 | uVar2;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x38))) =
         (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x38))) & 0xfffffffffffffe00 | (long)(int)(short)(iVar6 >> 1) & 0x1ffU;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x60))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x60))) & 0xfffffffffffffe00 | uVar2;
  }
  return 0;
}


RT_GUEST_FN(0x0026f450, guest::sceGsSetDefDBuffDc)

}  // namespace guest
