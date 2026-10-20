// guest:0x00249208 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t townMapBezel(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x70) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x74) = 0x00000000u;
  }
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
    uint32_t uStack_50 = rtguest::allocFrame(8);
    uint32_t uStack_48 = rtguest::allocFrame(8);
    uint32_t uStack_40 = rtguest::allocFrame(8);
  long lVar1;
  uint uVar2;
  uint uVar3;
  undefined8 uVar4;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01759be0)), 0, 400));
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x12000) == 0) {
    RT_CALLRA(0x249264, guest::uiArenaInit(0x1759be0,400));
    RT_CALLRA(0x249274, guest::uiArenaBeginBatch(0x1759be0,0,0));
    uVar4 = 10;
    if ((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) & 0x20) != 0) && (lVar1 = TestStateBitFc78(0x28), lVar1 != 0)) {
      uVar3 = 0;
      do {
        if ((*(int32_t*)rtguest::G(0x177fd88)) <= (int)uVar3) goto LAB_002492d4;
        uVar2 = uVar3 & 0xff;
        uVar3 = uVar3 + 1;
        lVar1 = RT_CALLRA(0x2492bc, guest::HasQueuedByteFlag(uVar2));
      } while (lVar1 != 0);
      uVar4 = 0xb;
    }
LAB_002492d4:
    RT_CALLRA(0x2492e0, guest::uiArenaSetTexture(0x1759be0,0x2a8fe0,0,uVar4));
    RT_CALLRA(0x2492ec, guest::uiArenaDataTag(0x1759be0,3));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xc0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x7800000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x249350, guest::uiArenaSprites(0x1759be0,uStack_80,1,0x1e,0x130,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_80)) = 0x80;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_78)) = 0xd8;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_70)) = 0x7800000010;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_68)) = 0;
    RT_CALLRA(0x2493b4, guest::uiArenaSprites(0x1759be0,uStack_80,1,0x1e,0x130,1));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_60)) = 0x10;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_58)) = 0xd0;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_50)) = 0x7800000070;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_48)) = 0x7800000008;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_40)) = 0;
    RT_CALLRA(0x24942c, guest::uiArenaSpritesWrap(0x1759be0,uStack_60,1,0x1e,0x130,1));
    RT_CALLRA(0x249434, guest::uiArenaSubmit(0x1759be0));
  }
  return 0;
}


RT_GUEST_FN(0x00249208, guest::townMapBezel)

}  // namespace guest
