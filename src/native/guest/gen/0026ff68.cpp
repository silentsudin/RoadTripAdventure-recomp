// guest:0x0026ff68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSetDefDrawEnv2(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  short sVar1;
  ulong uVar2;
  int iVar3;
  int iVar4;
  ulong uVar5;
  ulong uVar6;
  
  iVar4 = (int)((uint32_t)param_3 << 0x10) >> 0x10;
  uVar5 = (uint32_t)(int)param_2;
  iVar3 = (int)param_4;
  uVar6 = (uint32_t)(int)param_5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = 0x4d;
  (*(uint64_t*)rtguest::G((uint32_t)param_1)) = ((long)((int)((uint32_t)param_3 << 0x10) >> 0x16) & 0x3fU) << 0x10 | (uVar5 & 0xf) << 0x18;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = 0x4f;
  if (uVar6 == 0) {
    sVar1 = RT_CALLRA(0x26ffec, guest::sceGszbufaddr(uVar5,iVar4,iVar3));
    uVar2 = (long)sVar1 | ((long)(int)param_6 & 0xfU) << 0x18 | 0x100000000;
  }
  else {
    sVar1 = RT_CALLRA(0x270020, guest::sceGszbufaddr(uVar5,iVar4,iVar3));
    uVar2 = (long)sVar1 | ((long)(int)param_6 & 0xfU) << 0x18;
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar2;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint64_t))) = 0x19;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint64_t))) = (0x800 - (long)((short)param_3 >> 1)) * 0x10 | 0x800 - (long)(param_4 >> 1) << 0x24;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(7) * sizeof(uint64_t))) = 0x41;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(6) * sizeof(uint64_t))) = (long)(iVar4 + -1) << 0x10 | (long)(iVar3 + -1) << 0x30;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(9) * sizeof(uint64_t))) = 0x1a;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(8) * sizeof(uint64_t))) | 1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xb) * sizeof(uint64_t))) = 0x46;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(10) * sizeof(uint64_t))) | 1;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xd) * sizeof(uint64_t))) = 0x45;
  if ((uVar5 & 2) == 0) {
    uVar5 = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint64_t))) & 0xfffffffffffffffe;
  }
  else {
    uVar5 = (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint64_t))) | 1;
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xc) * sizeof(uint64_t))) = uVar5;
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xf) * sizeof(uint64_t))) = 0x48;
  if (uVar6 == 0) {
    uVar5 = 0x30000;
  }
  else {
    uVar5 = (uVar6 & 3) << 0x11 | 0x10000;
  }
  (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xe) * sizeof(uint64_t))) = uVar5;
  SYNC(0);
  return 8;
}


RT_GUEST_FN(0x0026ff68, guest::sceGsSetDefDrawEnv2)

}  // namespace guest
