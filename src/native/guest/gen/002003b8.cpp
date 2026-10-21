// guest:0x002003b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gameMainLoop(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x68, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x78, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x80, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x88, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x90, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x98, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0xa0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0xa8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xac) = 0x00000000u;
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
  }
uint32_t auStack_b0 = rtguest::allocFrame(16);
    uint32_t uStack_a0 = rtguest::allocFrame(8);
  undefined8 uVar1;
  long lVar2;
  ulong uVar3;
  uint uVar4;
  
  uVar3 = 0;
  uVar4 = 0;
  RT_CALLRA(0x2003fc, guest::__main());
  // Machine s-register seeds for this frame (gameMainLoop never pops): the
  // shadow file then flows into every callee's prologue sd spills. Each site
  // cited from machine words; a def in a call's delay slot is seeded BEFORE
  // that call (the delay slot runs before the callee's first spill).
  rtguest::gS[0] = 0x335900ull;  // 0x20040c addiu $16,$2,0x5900 (delay of jal 0x200408)
  RT_CALLRA(0x200408, guest::Ps2InitSystem());
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x00335900)), 0, 0x160));
  (*(uint8_t*)rtguest::G(0x33590e)) = 2;
  RT_CALLRA(0x200424, guest::loadFontTextures());
  RT_CALLRA(0x20042c, guest::uiBatchInit());
  RT_CALLRA(0x200434, guest::InitSoundRpcDriver());
  RT_CALLRA(0x20043c, guest::sceGsResetPath());
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x20100000)), 0, 0x40000));
  RT_CALLRA(0x200470, guest::sceGsSetDefLoadImage(auStack_b0,0,4,0,0,0,0x280,0x20));
  do {
    uVar4 = uVar4 + 1;
    (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_a0)) & 0xffffc000ffffffff | (uVar3 & 0x3fff) << 0x20;
    uVar3 = uVar3 + 0x140;
    RT_CALLRA(0x20049c, guest::FlushCache(0));
    RT_CALLRA(0x2004a8, guest::sceGsExecLoadImage(auStack_b0,0x100000));
    RT_CALLRA(0x2004b4, guest::sceGsSyncPath(0,0));
  } while (uVar4 < 0x15);
  rtguest::gS[7] = 0x1780000ull;  // 0x2004cc lui $23,0x178
  rtguest::gS[8] = 0x330000ull;   // 0x2004d0 lui $fp,0x33
  rtguest::gS[6] = 0x330000ull;   // 0x2004d8 lui $22,0x33 (delay of jal 0x2004d4)
  RT_CALLRA(0x2004d4, guest::gsInitDisplay((*(uint32_t*)rtguest::G(0x333288))));
  rtguest::gS[5] = 0x10000000ull;  // 0x2004e8 lui $21,0x1000 (delay of jal 0x2004e4)
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2e9b58u)));
  RT_CALLRA(0x2004ec, guest::ClearTaskPool(0x335900));
  RT_CALLRA(0x200500, guest::gs_tasks_spawn(0,0x2052d0,0));
  rtguest::gS[2] = 0x336180ull;  // 0x200514 addiu $18,$2,0x6180
  rtguest::gS[1] = 0x335e50ull;  // 0x200518 addiu $17,$3,0x5e50
  rtguest::gS[4] = 0x336010ull;  // 0x20051c addiu $20,$18,-0x170
  rtguest::gS[3] = 0x335ce0ull;  // 0x200520 addiu $19,$17,-0x170
  (*(uint8_t*)rtguest::G(0x335908)) = 0;
  do {
    RT_CALLRA(0x200528, guest::vifBeginPacket(0));
    RT_CALLRA(0x200534, guest::gs_tasks_walker(0x177a0d0,0x335900));
    RT_CALLRA(0x20053c, guest::uiBatchFlush((*(uint8_t*)rtguest::G(0x335908))));
    if (((*(uint32_t*)rtguest::G(0x335928)) & 0x8000) == 0) {
      RT_CALLRA(0x200560, guest::frameBlend((*(uint8_t*)rtguest::G(0x335908)) ^ 1,(int)(*(uint32_t*)rtguest::G(0x335928)) >> 0x16 & 1));
    }
    uVar1 = RT_CALLRA(0x200570, guest::sceDmaGetChan(1));
    RT_CALLRA(0x200580, guest::sceDmaSync(uVar1,0,0));
    uVar1 = RT_CALLRA(0x200588, guest::vifEndPacket(0));
    RT_CALLRA(0x200590, guest::vifKick(uVar1));
    RT_CALLRA(0x20059c, guest::sceGsSyncPath(0,0));
    lVar2 = RT_CALLRA(0x2005a4, guest::sceGsSyncV(0));
    REG_RCNT0_COUNT = 0;
    (*(uint8_t*)rtguest::G(0x335909)) = lVar2 == 0;
    if ((*(uint8_t*)rtguest::G(0x335908)) == 0) {
      RT_CALLRA(0x2005cc, guest::uiEnvCenterPos(0x336010,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x2005e0, guest::uiEnvCenterPosB(0x336090,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x2005f4, guest::uiEnvCenterPos(0x335ce0,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x200608, guest::uiEnvCenterPosB(0x335d60,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      (*(uint32_t*)rtguest::G(0x336130)) = (*(uint32_t*)rtguest::G(0x335950));
      (*(uint32_t*)rtguest::G((uint32_t)0x00335e00 + 0)) = (*(uint32_t*)rtguest::G(0x335950));
    }
    else {
      RT_CALLRA(0x20062c, guest::uiEnvCenterPos(0x336180,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x200640, guest::uiEnvCenterPosB(0x336200,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x200654, guest::uiEnvCenterPos(0x335e50,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      RT_CALLRA(0x200668, guest::uiEnvCenterPosB(0x335ed0,0x800,0x800,(*(uint8_t*)rtguest::G(0x335909))));
      (*(uint32_t*)rtguest::G(0x3362a0)) = (*(uint32_t*)rtguest::G(0x335950));
      (*(uint32_t*)rtguest::G((uint32_t)0x00335f70 + 0)) = (*(uint32_t*)rtguest::G(0x335950));
    }
    RT_CALLRA(0x20067c, guest::FlushCache(0));
    if (((*(uint32_t*)rtguest::G(0x335928)) & 0x400000) == 0) {
      RT_CALLRA(0x200698, guest::sceGsSwapDBuffDc(0x335c80,(*(uint8_t*)rtguest::G(0x335908))));
    }
    else {
      RT_CALLRA(0x2006a8, guest::sceGsSwapDBuffDc(0x335fb0,(*(uint8_t*)rtguest::G(0x335908))));
    }
    RT_CALLRA(0x2006b4, guest::sceGsSyncPath(0,0));
    (*(uint32_t*)rtguest::G(0x335928)) = (*(uint32_t*)rtguest::G(0x335928)) & 0xffbfffff;
    (*(uint32_t*)rtguest::G(0x335900)) = (*(uint32_t*)rtguest::G(0x335900)) + 1;
    (*(uint8_t*)rtguest::G(0x335908)) = (*(uint8_t*)rtguest::G(0x335908)) ^ 1;
  } while( true );
}


RT_GUEST_FN(0x002003b8, guest::gameMainLoop)

}  // namespace guest
