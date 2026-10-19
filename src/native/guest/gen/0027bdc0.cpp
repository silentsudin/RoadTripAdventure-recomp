// guest:0x0027bdc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dpdiv(uint64_t param_1, uint64_t param_2, ...) {
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
    uint32_t iStack_58 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_50 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_40 = rtguest::allocFrame(4);
    uint32_t uStack_3c = rtguest::allocFrame(4);
    uint32_t iStack_38 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(8);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
  bool bVar1;
  ulong uVar2;
uint32_t puVar3;
  ulong uVar4;
  
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_2;
  __unpack_d(uStack_20,uStack_60);
  __unpack_d(uStack_18,uStack_40);
  puVar3 = uStack_60;
  uVar4 = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  if ((1 < (*(uint32_t*)rtguest::G((uint32_t)uStack_60))) && (puVar3 = uStack_40, 1 < (*(uint32_t*)rtguest::G((uint32_t)uStack_40)))) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) ^ (*(uint32_t*)rtguest::G((uint32_t)uStack_3c));
    if (((*(uint32_t*)rtguest::G((uint32_t)uStack_60)) == 4) || ((*(uint32_t*)rtguest::G((uint32_t)uStack_60)) == 2)) {
      puVar3 = uStack_60;
      if ((*(uint32_t*)rtguest::G((uint32_t)uStack_60)) == (*(uint32_t*)rtguest::G((uint32_t)uStack_40))) {
        puVar3 = (uint32_t)0x00331f70;
      }
    }
    else if ((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) == 4) {
      (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0;
      (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = 0;
      puVar3 = uStack_60;
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
    }
    else if ((*(uint32_t*)rtguest::G((uint32_t)uStack_40)) == 2) {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = 4;
      puVar3 = uStack_60;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = (*(int32_t*)rtguest::G((uint32_t)iStack_58)) - (*(int32_t*)rtguest::G((uint32_t)iStack_38));
      bVar1 = false;
      if ((*(uint64_t*)rtguest::G((uint32_t)uStack_50)) < (*(uint64_t*)rtguest::G((uint32_t)uStack_30))) {
        (*(int32_t*)rtguest::G((uint32_t)iStack_58)) = (*(int32_t*)rtguest::G((uint32_t)iStack_58)) + -1;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) << 1;
        bVar1 = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) < (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
      }
      uVar2 = 0x1000000000000000;
      uVar4 = 0;
      while( true ) {
        if (!bVar1) {
          uVar4 = uVar4 | uVar2;
          (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) - (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
        }
        uVar2 = uVar2 >> 1;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) << 1;
        if (uVar2 == 0) break;
        bVar1 = (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) < (*(uint64_t*)rtguest::G((uint32_t)uStack_30));
      }
      puVar3 = uStack_60;
      if ((uVar4 & 0xff) == 0x80) {
        if ((uVar4 & 0x100) == 0) {
          puVar3 = uStack_60;
          if ((*(uint64_t*)rtguest::G((uint32_t)uStack_50)) != 0) {
            puVar3 = uStack_60;
            uVar4 = uVar4 + 0x80;
          }
        }
        else {
          puVar3 = uStack_60;
          uVar4 = uVar4 + 0x80;
        }
      }
    }
  }
  (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = uVar4;
  __pack_d(puVar3);
  return 0;
}


RT_GUEST_FN(0x0027bdc0, guest::dpdiv)

}  // namespace guest
