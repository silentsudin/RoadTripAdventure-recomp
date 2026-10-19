// guest:0x00230c20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudElementF(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
    uint32_t iStack_90 = rtguest::allocFrame(4);
    uint32_t iStack_f4 = rtguest::allocFrame(4);
    uint32_t iStack_f8 = rtguest::allocFrame(4);
    uint32_t uStack_100 = rtguest::allocFrame(4);
    uint32_t uStack_108 = rtguest::allocFrame(8);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_118 = rtguest::allocFrame(8);
    uint32_t uStack_120 = rtguest::allocFrame(8);
    uint32_t uStack_94 = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(4);
    uint32_t uStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t uStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a8 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t uStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_bc = rtguest::allocFrame(4);
    uint32_t uStack_c0 = rtguest::allocFrame(4);
    uint32_t uStack_c4 = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_d4 = rtguest::allocFrame(4);
    uint32_t uStack_d8 = rtguest::allocFrame(4);
    uint32_t uStack_dc = rtguest::allocFrame(4);
    uint32_t uStack_e0 = rtguest::allocFrame(4);
    uint32_t uStack_e4 = rtguest::allocFrame(4);
    uint32_t uStack_e8 = rtguest::allocFrame(4);
    uint32_t uStack_ec = rtguest::allocFrame(4);
    uint32_t uStack_f0 = rtguest::allocFrame(4);
    uint32_t uStack_fc = rtguest::allocFrame(4);
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = (int)param_3 + -1;
  if (param_3 != 0) {
    uVar5 = 0xc2200000;
    uVar4 = 0xc2400000;
    do {
      if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 1) {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0xffffffe8ffffffec;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x30000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,0,2);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_120,1,iVar1,iVar2,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0xffffffe8ffffffec;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = CONCAT44(((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) / 6) * 0x30,((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t))) % 6) * 0x28);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,0,1);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_120,1,iVar1 + -0x28,iVar2,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0xffffffe8ffffffec;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = CONCAT44(((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))) / 6) * 0x30,((*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t))) % 6) * 0x28);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        uiArenaSetTexture(param_1,0x29f5f8,0,0);
        uiArenaDataTag(param_1,1);
        uiArenaSprites(param_1,uStack_120,1,iVar1 + 0x28,iVar2,0);
      }
      else if ((*(int32_t*)rtguest::G((uint32_t)param_2)) == 2) {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(int32_t)));
        iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(int32_t)));
        uiArenaSetTexture(param_1,0x29f5f8,0,2);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = 0x30000000a0;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42200000;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f4)) = 0x30;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x42400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x28;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0x30;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = 0xc2200000;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f8)) = 0xa0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = 0xc2400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_dc));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_e0));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_d0));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_bc));
        uiArenaRotQuads(param_1,uStack_100,1,iVar1,iVar2,0);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(int32_t)));
        uiArenaSetTexture(param_1,0x29f5f8,0,1);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f8)) = (iVar3 % 6) * 0x28;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f4)) = (iVar3 / 6) * 0x30;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_f4)),(*(int32_t*)rtguest::G((uint32_t)iStack_f8)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x28;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0x30;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42200000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x42400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42200000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x42400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = uVar5;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = uVar5;
        uiArenaRotQuads(param_1,uStack_100,1,iVar1 + -0xa0,iVar2,0);
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(4) * sizeof(int32_t)));
        uiArenaSetTexture(param_1,0x29f5f8,0,0);
        (*(uint64_t*)rtguest::G((uint32_t)uStack_120)) = 0;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f8)) = (iVar3 % 6) * 0x28;
        (*(int32_t*)rtguest::G((uint32_t)iStack_f4)) = (iVar3 / 6) * 0x30;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_118)) = CONCAT44((*(int32_t*)rtguest::G((uint32_t)iStack_f4)),(*(int32_t*)rtguest::G((uint32_t)iStack_f8)));
        (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = 0x3000000028;
        (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_100)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_fc)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_f0)) = 0x28;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0x30;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e4)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 0x42200000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_bc)) = 0x42400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_b0)) = 0x42200000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0x42400000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a8)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_9c)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_98)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_94)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_e0)) = uVar5;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_dc)) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_c0)) = uVar5;
        uiArenaRotQuads(param_1,uStack_100,1,iVar1 + 0xa0,iVar2,0);
      }
      param_2 = param_2 + 5;
      (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = (*(int32_t*)rtguest::G((uint32_t)iStack_90)) + -1;
    } while ((*(int32_t*)rtguest::G((uint32_t)iStack_90)) != -1);
  }
  return 0;
}


RT_GUEST_FN(0x00230c20, guest::raceHudElementF)

}  // namespace guest
