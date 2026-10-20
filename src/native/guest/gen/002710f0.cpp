// guest:0x002710f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridDraw(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(224);
uint32_t auStack_e0 = rtguest::allocFrame(4);
uint32_t auStack_dc = rtguest::allocFrame(12);
uint32_t auStack_d0 = rtguest::allocFrame(16);
rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_b8 = rtguest::allocFrame(4);
    uint32_t uStack_b4 = rtguest::allocFrame(4);
  uint uVar1;
  uint uVar2;
  uint uVar3;
uint32_t puVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
uint32_t puStack_c0;
uint32_t puStack_bc;
uint32_t puStack_b0;
  
  uVar8 = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = 0;
  puStack_c0 = param_1;
  puStack_bc = (uint32_t)sceDmaGetChan(2);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) = (*(uint32_t*)rtguest::G((uint32_t)puStack_c0 + (uint32_t)(1) * sizeof(uint32_t)));
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puStack_c0));
  uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puStack_c0 + (uint32_t)(2) * sizeof(uint32_t)));
  gifPkBufferInit(auStack_e0,0x70000000);
  gifPkBufferInit(auStack_d0,0x70002000);
  (*(uint32_t*)rtguest::G((uint32_t)puStack_bc)) = (*(uint32_t*)rtguest::G((uint32_t)puStack_bc)) | 0x40;
  if ((*(uint32_t*)rtguest::G((uint32_t)uStack_b8)) != 0) {
    do {
      if (uVar1 != 0) {
        puStack_b0 = puStack_c0 + 6;
        uVar5 = 0;
        do {
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)uStack_b4));
          puVar4 = auStack_e0 + (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) * 0x10;
          uVar3 = uVar1 - uVar5;
          if (4 < uVar1 - uVar5) {
            uVar3 = 4;
          }
          gifPkBufferReset(puVar4);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_b4)) ^ 1;
          uVar6 = uVar5 + uVar3;
          textEmitGlyphsToGs(puVar4,puStack_b0,uVar5,uVar8,uVar7,uVar3);
          gifPkAppendTagNreg7(puVar4,0,0,0);
          gifPkAlign16(puVar4);
          sceGsSyncPath(0,0);
          sceDmaSend(puStack_bc,(*(uint32_t*)rtguest::G((uint32_t)auStack_dc + (uint32_t)(uVar2 * 4) * 4)) & 0x3fff | 0x80000000);
          uVar7 = uVar7 + uVar3 * 2;
          uVar5 = uVar6;
        } while (uVar6 < uVar1);
      }
      uVar8 = uVar8 + 1;
    } while (uVar8 < (*(uint32_t*)rtguest::G((uint32_t)uStack_b8)));
  }
  sceGsSyncPath(0,0);
  return 0;
}


RT_GUEST_FN(0x002710f0, guest::textGridDraw)

}  // namespace guest
