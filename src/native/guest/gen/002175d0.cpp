// guest:0x002175d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t TestCarCollisionBox(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
uint32_t afStack_30 = rtguest::allocFrame(8);
    uint32_t fStack_28 = rtguest::allocFrame(4);
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = param_2 + 0x90;
  param_2 = param_2 + 0xb0;
  RT_CALLRA(0x2175fc, guest::vu0SubVector(afStack_30,param_1 + 0x140,iVar2));
  RT_CALLRA(0x21760c, guest::vu0ApplyMatrix(afStack_30,param_2,afStack_30));
  if (((((*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) < (*(float*)rtguest::G(0x332b4c))) || ((*(uint32_t*)rtguest::G(0x332b50)) < (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)))) || ((*(float*)rtguest::G((uint32_t)fStack_28)) < -1.5)) ||
     (uVar1 = 1, 1.5 < (*(float*)rtguest::G((uint32_t)fStack_28)))) {
    RT_CALLRA(0x217684, guest::vu0SubVector(afStack_30,param_1 + 0x150,iVar2));
    RT_CALLRA(0x217694, guest::vu0ApplyMatrix(afStack_30,param_2,afStack_30));
    if ((((*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) < (*(uint32_t*)rtguest::G(0x332b54))) || ((*(uint32_t*)rtguest::G(0x332b58)) < (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)))) ||
       (((*(float*)rtguest::G((uint32_t)fStack_28)) < -1.5 || (uVar1 = 1, 1.5 < (*(float*)rtguest::G((uint32_t)fStack_28)))))) {
      RT_CALLRA(0x21770c, guest::vu0SubVector(afStack_30,param_1 + 0x160,iVar2));
      RT_CALLRA(0x21771c, guest::vu0ApplyMatrix(afStack_30,param_2,afStack_30));
      if ((((*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) < (*(uint32_t*)rtguest::G(0x332b5c))) || ((*(uint32_t*)rtguest::G(0x332b60)) < (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)))) ||
         (((*(float*)rtguest::G((uint32_t)fStack_28)) < -1.5 || (uVar1 = 1, 1.5 < (*(float*)rtguest::G((uint32_t)fStack_28)))))) {
        RT_CALLRA(0x217794, guest::vu0SubVector(afStack_30,param_1 + 0x170,iVar2));
        RT_CALLRA(0x2177a4, guest::vu0ApplyMatrix(afStack_30,param_2,afStack_30));
        uVar1 = 0;
        if (((((*(uint32_t*)rtguest::G(0x332b64)) <= (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4))) && ((*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) <= (*(uint32_t*)rtguest::G(0x332b68)))) &&
            (-1.5 <= (*(float*)rtguest::G((uint32_t)fStack_28)))) && (uVar1 = 1, 1.5 < (*(float*)rtguest::G((uint32_t)fStack_28)))) {
          uVar1 = 0;
        }
      }
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x002175d0, guest::TestCarCollisionBox)

}  // namespace guest
