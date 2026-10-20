// guest:0x002575e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcRegisterFiles(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(144);
    uint32_t uStack_74 = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint32_t)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint32_t)));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)uStack_74)), (char *)rtguest::G((uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_2))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44(uVar1,param_1 * 2);
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_74)),uVar2);
  sceSifCallRpc(0x175e840,0x32,0,0x175e880,0x40,0,0,0);
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint32_t)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint32_t)));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)uStack_74)), (char *)rtguest::G((uint32_t)((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44(uVar1,param_1 * 2 + 1);
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_74)),uVar2);
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_70));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_68));
  (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60));
  (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  sceSifCallRpc(0x175e840,0x32,0,0x175e880,0x40,0,0,0);
  return 0;
}


RT_GUEST_FN(0x002575e0, guest::SoundRpcRegisterFiles)

}  // namespace guest
