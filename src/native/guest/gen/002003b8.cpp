// guest:0x002003b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gameMainLoop(uint32_t q0, ...) {
uint32_t auStack_b0 = rtguest::allocFrame(16);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
  undefined8 uVar1;
  long lVar2;
  ulong uVar3;
  uint uVar4;
  
  uVar3 = 0;
  uVar4 = 0;
  __main();
  Ps2InitSystem();
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x00335900)), 0, 0x160));
  (*(uint8_t*)rtguest::G(0x33590e)) = 2;
  loadFontTextures();
  uiBatchInit();
  InitSoundRpcDriver();
  sceGsResetPath();
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x20100000)), 0, 0x40000));
  sceGsSetDefLoadImage(auStack_b0,0,4,0,0,0,0x280,0x20);
  do {
    uVar4 = uVar4 + 1;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) & 0xffffc000ffffffff | (uVar3 & 0x3fff) << 0x20;
    uVar3 = uVar3 + 0x140;
    FlushCache(0);
    sceGsExecLoadImage(auStack_b0,0x100000);
    sceGsSyncPath(0,0);
  } while (uVar4 < 0x15);
  gsInitDisplay((*(uint32_t*)rtguest::G(0x333288)));
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9b58u)));
  ClearTaskPool(0x335900);
  gs_tasks_spawn(0,0x2052d0,0);
  (*(uint8_t*)rtguest::G(0x335908)) = 0;
  do {
    vifBeginPacket(0);
    gs_tasks_walker(0x177a0d0,0x335900);
    uiBatchFlush((*(uint8_t*)rtguest::G(0x335908)));
    if (((*(uint32_t*)rtguest::G(0x335928)) & 0x8000) == 0) {
      frameBlend((*(uint8_t*)rtguest::G(0x335908)) ^ 1,(int)(*(uint32_t*)rtguest::G(0x335928)) >> 0x16 & 1);
    }
    uVar1 = sceDmaGetChan(1);
    sceDmaSync(uVar1,0,0);
    uVar1 = vifEndPacket(0);
    vifKick(uVar1);
    sceGsSyncPath(0,0);
    lVar2 = sceGsSyncV(0);
    REG_RCNT0_COUNT = 0;
    (*(uint8_t*)rtguest::G(0x335909)) = lVar2 == 0;
    if ((*(uint8_t*)rtguest::G(0x335908)) == 0) {
      uiEnvCenterPos(0x336010,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPosB(0x336090,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPos(0x335ce0,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPosB(0x335d60,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      (*(uint32_t*)rtguest::G(0x336130)) = (*(uint32_t*)rtguest::G(0x335950));
      (*(uint32_t*)rtguest::G((uint32_t)0x00335e00 + 0)) = (*(uint32_t*)rtguest::G(0x335950));
    }
    else {
      uiEnvCenterPos(0x336180,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPosB(0x336200,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPos(0x335e50,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      uiEnvCenterPosB(0x335ed0,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909)));
      (*(uint32_t*)rtguest::G(0x3362a0)) = (*(uint32_t*)rtguest::G(0x335950));
      (*(uint32_t*)rtguest::G((uint32_t)0x00335f70 + 0)) = (*(uint32_t*)rtguest::G(0x335950));
    }
    FlushCache(0);
    if (((*(uint32_t*)rtguest::G(0x335928)) & 0x400000) == 0) {
      sceGsSwapDBuffDc(0x335c80,(*(uint8_t*)rtguest::G(0x335908)));
    }
    else {
      sceGsSwapDBuffDc(0x335fb0,(*(uint8_t*)rtguest::G(0x335908)));
    }
    sceGsSyncPath(0,0);
    (*(uint32_t*)rtguest::G(0x335928)) = (*(uint32_t*)rtguest::G(0x335928)) & 0xffbfffff;
    (*(uint32_t*)rtguest::G(0x335900)) = (*(uint32_t*)rtguest::G(0x335900)) + 1;
    (*(uint8_t*)rtguest::G(0x335908)) = (*(uint8_t*)rtguest::G(0x335908)) ^ 1;
  } while( true );
}


RT_GUEST_FN(0x002003b8, guest::gameMainLoop)

}  // namespace guest
