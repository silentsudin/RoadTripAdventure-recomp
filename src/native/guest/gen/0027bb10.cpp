// guest:0x0027bb10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dpmul(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t iStack_68 = rtguest::allocFrame(4);
    uint32_t iStack_88 = rtguest::allocFrame(4);
    uint32_t iStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
  ulong uVar1;
  ulong uVar2;
  long lVar3;
uint32_t puVar4;
  ulong uVar5;
  ulong uVar6;
  
  puVar4 = uStack_b0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = param_2;
  __unpack_d(uStack_50,uStack_b0);
  __unpack_d(uStack_48,uStack_90);
  if ((*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) < 2) {
LAB_0027bbc4:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) != (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)));
  }
  else {
    if (1 < (*(uint32_t*)rtguest::G((uint32_t)uStack_90))) {
      if ((*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) == 4) {
        if ((*(uint32_t*)rtguest::G((uint32_t)uStack_90)) == 2) {
LAB_0027bbac:
          puVar4 = (uint32_t)0x00331f70;
          goto LAB_0027bd8c;
        }
        goto LAB_0027bbc4;
      }
      if ((*(uint32_t*)rtguest::G((uint32_t)uStack_90)) == 4) {
        if (((*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) ^ 2) == 0) goto LAB_0027bbac;
      }
      else {
        if (((*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) ^ 2) == 0) goto LAB_0027bbc4;
        if ((*(uint32_t*)rtguest::G((uint32_t)uStack_90)) != 2) {
          uVar1 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)uStack_80)) & 0xffffffff,(*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) & 0xffffffff);
          uVar2 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)uStack_80)) >> 0x20,(*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) & 0xffffffff);
          lVar3 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)uStack_80)) & 0xffffffff,(*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) >> 0x20);
          uVar6 = uVar2 + lVar3;
          lVar3 = __muldi3((*(uint64_t*)rtguest::G((uint32_t)uStack_80)) >> 0x20,(*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) >> 0x20);
          uVar5 = uVar1 + (uVar6 << 0x20);
          (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = ((uint32_t)(uVar6 < uVar2) << 0x20) + (uint32_t)(uVar5 < uVar1) +
                      (uVar6 >> 0x20) + lVar3;
          (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + (*(int32_t*)rtguest::G((uint32_t)iStack_88)) + 4;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) != (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)));
          while (0x1fffffffffffffff < (*(uint64_t*)rtguest::G((uint32_t)uStack_60))) {
            uVar1 = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) & 1;
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) >> 1;
            (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68)) + 1;
            if (uVar1 != 0) {
              uVar5 = uVar5 >> 1 | 0x8000000000000000;
            }
          }
          while ((*(uint64_t*)rtguest::G((uint32_t)uStack_60)) < 0x1000000000000000) {
            (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) << 1;
            (*(int32_t*)rtguest::G((uint32_t)iStack_68)) = (*(int32_t*)rtguest::G((uint32_t)iStack_68)) + -1;
            if ((uVar5 & 0x8000000000000000) != 0) {
              (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) | 1;
            }
            uVar5 = uVar5 << 1;
          }
          if (((*(uint64_t*)rtguest::G((uint32_t)uStack_60)) & 0xff) == 0x80) {
            if (((*(uint64_t*)rtguest::G((uint32_t)uStack_60)) & 0x100) == 0) {
              if (uVar5 != 0) {
                (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) + 0x80;
              }
            }
            else {
              (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) + 0x80;
            }
          }
          (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = 3;
          puVar4 = uStack_70;
          goto LAB_0027bd8c;
        }
      }
    }
    (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = (uint32_t)((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) != (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)));
    puVar4 = uStack_90;
  }
LAB_0027bd8c:
  __pack_d(puVar4);
  return 0;
}


RT_GUEST_FN(0x0027bb10, guest::dpmul)

}  // namespace guest
