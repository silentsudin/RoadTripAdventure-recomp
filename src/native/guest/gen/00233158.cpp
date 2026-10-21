// guest:0x00233158 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetObjectState(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
uint32_t __s;
  int iVar3;
uint32_t puVar4;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008)));
  if (iVar2 == 0x19) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004)));
    iVar3 = 2;
    if (param_3 == 0) {
      iVar3 = 0;
    }
    __s = (uint32_t)(param_2 * 0x408 + param_1 + 0x8748);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)__s));
    puVar4 = (uint32_t)0x29f9e8;
  }
  else {
    iVar1 = param_2 << 7;
    if ((iVar2 < 0x1a) && (iVar1 = param_2 * 0x80, iVar2 == 0x14)) {
      iVar3 = 2;
      iVar2 = param_2 * 0x408 + param_1;
      if (param_3 == 0) {
        iVar3 = 0;
      }
      __s = (uint32_t)(iVar2 + 0x8748);
      param_2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) + param_2;
      puVar4 = 0x0029f9c8;
      if ((*(int32_t*)rtguest::G((uint32_t)__s)) == iVar3) {
        return 0;
      }
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
      (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x874c))) = 0x140;
      if (param_2 == 0) {
        iVar3 = 0xa8;
      }
      else {
        iVar3 = 0x70;
        if (param_2 != 1) {
          iVar3 = 0x150;
        }
      }
      iVar2 = 3;
      goto LAB_00233300;
    }
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004)));
    iVar3 = 2;
    if (param_3 == 0) {
      iVar3 = 0;
    }
    __s = (uint32_t)((iVar1 + param_2) * 8 + param_1 + 0x8748);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)__s));
    puVar4 = 0x0029fa08;
  }
  if (iVar1 == iVar3) {
    return 0;
  }
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x2c));
  (*(int32_t*)rtguest::G((uint32_t)__s)) = iVar3;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(1) * sizeof(int32_t))) = 0x140;
  if (iVar2 + param_2 == 0) {
    iVar3 = 0xa8;
  }
  else {
    iVar3 = 0x70;
    if (iVar2 + param_2 != 1) {
      iVar3 = 0x150;
    }
  }
  iVar2 = 1;
LAB_00233300:
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(2) * sizeof(int32_t))) = iVar3;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(10) * sizeof(int32_t))) = 0x40000000;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(3) * sizeof(int32_t))) = 1;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(4) * sizeof(int32_t))) = iVar2;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(5) * sizeof(int32_t))) = (int)puVar4;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(6) * sizeof(int32_t))) = 1;
  (*(int32_t*)rtguest::G((uint32_t)__s + (uint32_t)(9) * sizeof(int32_t))) = 0x1e;
  return 0;
}


RT_GUEST_FN(0x00233158, guest::SetObjectState)

}  // namespace guest
