// guest:0x00256af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitSoundRpcDriver(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(1120);
    rtguest::allocFrame(528);  // guest frame gap (sp+0x0..0x20f)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x430, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x410, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x418, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x420, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x428, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x438, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x440, 8);
    rtguest::rtSpillS(rt_fg, 7, rt_sv, 0x448, 8);
    rtguest::rtSpillS(rt_fg, 8, rt_sv, 0x450, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x458) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x45c) = 0x00000000u;
    rtguest::gS[0] = 0x000000000175e880ull;  // walk const
    rtguest::gS[3] = 0x000000000000003full;  // walk const
    rtguest::gS[4] = 0x000000000175e840ull;  // walk const
    rtguest::gS[5] = 0x0000000000003fffull;  // walk const
    rtguest::gS[6] = 0x0000000000160000ull;  // walk const
  }
    uint32_t uStack_250 = rtguest::allocFrame(4);  // sp+0x210 (Ghidra entry-sp -0x250, frame 0x460)
    uint32_t uStack_24c = rtguest::allocFrame(4);  // sp+0x214 (Ghidra entry-sp -0x24c, frame 0x460)
    uint32_t uStack_248 = rtguest::allocFrame(2);  // sp+0x218 (Ghidra entry-sp -0x248, frame 0x460)
    uint32_t uStack_246 = rtguest::allocFrame(2);  // sp+0x21a (Ghidra entry-sp -0x246, frame 0x460)
    uint32_t uStack_244 = rtguest::allocFrame(4);  // sp+0x21c (Ghidra entry-sp -0x244, frame 0x460)
    uint32_t uStack_240 = rtguest::allocFrame(4);  // sp+0x220 (Ghidra entry-sp -0x240, frame 0x460)
    rtguest::allocFrame(12);  // guest frame gap (sp+0x224..0x22f)
    uint32_t uStack_230 = rtguest::allocFrame(4);  // sp+0x230 (Ghidra entry-sp -0x230, frame 0x460)
    uint32_t uStack_22c = rtguest::allocFrame(4);  // sp+0x234 (Ghidra entry-sp -0x22c, frame 0x460)
    uint32_t uStack_228 = rtguest::allocFrame(2);  // sp+0x238 (Ghidra entry-sp -0x228, frame 0x460)
    uint32_t uStack_226 = rtguest::allocFrame(2);  // sp+0x23a (Ghidra entry-sp -0x226, frame 0x460)
    uint32_t uStack_224 = rtguest::allocFrame(4);  // sp+0x23c (Ghidra entry-sp -0x224, frame 0x460)
    uint32_t uStack_220 = rtguest::allocFrame(4);  // sp+0x240 (Ghidra entry-sp -0x220, frame 0x460)
    rtguest::allocFrame(12);  // guest frame gap (sp+0x244..0x24f)
    uint32_t uStack_210 = rtguest::allocFrame(4);  // sp+0x250 (Ghidra entry-sp -0x210, frame 0x460)
    uint32_t uStack_20c = rtguest::allocFrame(4);  // sp+0x254 (Ghidra entry-sp -0x20c, frame 0x460)
    uint32_t uStack_208 = rtguest::allocFrame(4);  // sp+0x258 (Ghidra entry-sp -0x208, frame 0x460)
uint32_t auStack_204 = rtguest::allocFrame(52);  // sp+0x25c (Ghidra entry-sp -0x204, frame 0x460)
    uint32_t uStack_1d0 = rtguest::allocFrame(4);  // sp+0x290 (Ghidra entry-sp -0x1d0, frame 0x460)
    uint32_t uStack_1cc = rtguest::allocFrame(4);  // sp+0x294 (Ghidra entry-sp -0x1cc, frame 0x460)
    uint32_t uStack_1c8 = rtguest::allocFrame(4);  // sp+0x298 (Ghidra entry-sp -0x1c8, frame 0x460)
uint32_t auStack_1c4 = rtguest::allocFrame(52);  // sp+0x29c (Ghidra entry-sp -0x1c4, frame 0x460)
    uint32_t uStack_190 = rtguest::allocFrame(4);  // sp+0x2d0 (Ghidra entry-sp -0x190, frame 0x460)
    uint32_t uStack_18c = rtguest::allocFrame(4);  // sp+0x2d4 (Ghidra entry-sp -0x18c, frame 0x460)
    uint32_t uStack_188 = rtguest::allocFrame(4);  // sp+0x2d8 (Ghidra entry-sp -0x188, frame 0x460)
uint32_t auStack_184 = rtguest::allocFrame(52);  // sp+0x2dc (Ghidra entry-sp -0x184, frame 0x460)
    uint32_t uStack_150 = rtguest::allocFrame(4);  // sp+0x310 (Ghidra entry-sp -0x150, frame 0x460)
    uint32_t uStack_14c = rtguest::allocFrame(4);  // sp+0x314 (Ghidra entry-sp -0x14c, frame 0x460)
    uint32_t uStack_148 = rtguest::allocFrame(4);  // sp+0x318 (Ghidra entry-sp -0x148, frame 0x460)
uint32_t auStack_144 = rtguest::allocFrame(52);  // sp+0x31c (Ghidra entry-sp -0x144, frame 0x460)
    uint32_t uStack_110 = rtguest::allocFrame(4);  // sp+0x350 (Ghidra entry-sp -0x110, frame 0x460)
    uint32_t uStack_10c = rtguest::allocFrame(4);  // sp+0x354 (Ghidra entry-sp -0x10c, frame 0x460)
    uint32_t uStack_108 = rtguest::allocFrame(4);  // sp+0x358 (Ghidra entry-sp -0x108, frame 0x460)
uint32_t auStack_104 = rtguest::allocFrame(52);  // sp+0x35c (Ghidra entry-sp -0x104, frame 0x460)
    uint32_t uStack_d0 = rtguest::allocFrame(4);  // sp+0x390 (Ghidra entry-sp -0xd0, frame 0x460)
    uint32_t uStack_cc = rtguest::allocFrame(4);  // sp+0x394 (Ghidra entry-sp -0xcc, frame 0x460)
    uint32_t uStack_c8 = rtguest::allocFrame(4);  // sp+0x398 (Ghidra entry-sp -0xc8, frame 0x460)
uint32_t auStack_c4 = rtguest::allocFrame(52);  // sp+0x39c (Ghidra entry-sp -0xc4, frame 0x460)
    uint32_t uStack_90 = rtguest::allocFrame(4);  // sp+0x3d0 (Ghidra entry-sp -0x90, frame 0x460)
    uint32_t uStack_8c = rtguest::allocFrame(4);  // sp+0x3d4 (Ghidra entry-sp -0x8c, frame 0x460)
    uint32_t uStack_88 = rtguest::allocFrame(4);  // sp+0x3d8 (Ghidra entry-sp -0x88, frame 0x460)
    uint32_t uStack_84 = rtguest::allocFrame(4);  // sp+0x3dc (Ghidra entry-sp -0x84, frame 0x460)
    uint32_t uStack_80 = rtguest::allocFrame(8);  // sp+0x3e0 (Ghidra entry-sp -0x80, frame 0x460)
    uint32_t uStack_78 = rtguest::allocFrame(8);  // sp+0x3e8 (Ghidra entry-sp -0x78, frame 0x460)
    uint32_t uStack_70 = rtguest::allocFrame(8);  // sp+0x3f0 (Ghidra entry-sp -0x70, frame 0x460)
    uint32_t uStack_68 = rtguest::allocFrame(8);  // sp+0x3f8 (Ghidra entry-sp -0x68, frame 0x460)
    uint32_t uStack_60 = rtguest::allocFrame(8);  // sp+0x400 (Ghidra entry-sp -0x60, frame 0x460)
    uint32_t uStack_58 = rtguest::allocFrame(8);  // sp+0x408 (Ghidra entry-sp -0x58, frame 0x460)
  
  RT_CALLRA(0x256b40, guest::sceSdRemoteInit());
  RT_CALLRA(0x256b54, guest::sceSifBindRpc(0x175e840,0x1234567,0));
  RT_CALLRA(0x256b7c, guest::sceSifCallRpc(0x175e840,0,0,0,0,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0;
  RT_CALLRA(0x256bec, guest::sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x1000000000001;
  RT_CALLRA(0x256c64, guest::sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x2000000000002;
  RT_CALLRA(0x256cdc, guest::sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x3000000000003;
  RT_CALLRA(0x256d54, guest::sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x1b000000000000;
  RT_CALLRA(0x256dc8, guest::sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x16000000000001;
  RT_CALLRA(0x256e3c, guest::sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x16000000000002;
  RT_CALLRA(0x256eb0, guest::sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0));
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x10500000000003;
  RT_CALLRA(0x256f3c, guest::sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0));
  RT_CALLRA(0x256f54, guest::sceSdRemote(1,0x8050,0x1d00,0x15ffff));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_24c)) = 0x103;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_248)) = 0x3fff;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_246)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_244)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_240)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_250)) = 0;
  RT_CALLRA(0x256f84, guest::sceSdRemote(1,0x8130,0,uStack_250));
  RT_CALLRA(0x256f98, guest::sceSdRemote(1,0x8070,2,1));
  RT_CALLRA(0x256fb0, guest::sceSdRemote(1,0x8050,0x1d01,0x1fffff));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_22c)) = 0x103;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_226)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_220)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_230)) = 1;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_228)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_224)) = 0x3f;
  RT_CALLRA(0x256fe0, guest::sceSdRemote(1,0x8130,1,uStack_230));
  RT_CALLRA(0x256ff4, guest::sceSdRemote(1,0x8070,3,1));
  RT_CALLRA(0x257004, guest::SoundRpcRegisterFiles(0,0x29ae20,0x29ae2c));
  RT_CALLRA(0x257014, guest::SoundRpcRegisterFiles(2,0x29ae38,0x29ae44));
  RT_CALLRA(0x257024, guest::SoundRpcRegisterFiles(1,0x29ae50,0x29ae5c));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_210)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20c)) = (*(uint32_t*)rtguest::G(0x29adf4));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_208)) = (*(uint32_t*)rtguest::G(0x29adf8));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_210)) = 0;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_204)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x29adf0))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_20c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_210)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_204 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_208)));
  RT_CALLRA(0x2570c4, guest::sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_1d0)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1cc)) = (*(uint32_t*)rtguest::G(0x29ae00));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c8)) = (*(uint32_t*)rtguest::G(0x29ae04));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1d0)) = 0;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_1c4)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029adfc))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_1cc)),(*(uint32_t*)rtguest::G((uint32_t)uStack_1d0)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_1c4 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_1c8)));
  RT_CALLRA(0x25715c, guest::sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_190)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_188)) = (*(uint32_t*)rtguest::G(0x29ae10));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18c)) = (*(uint32_t*)rtguest::G(0x29ae0c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_190)) = 3;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_184)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae08))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_18c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_190)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_184 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_188)));
  RT_CALLRA(0x2571f8, guest::sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_150)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_150)) = 3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14c)) = (*(uint32_t*)rtguest::G(0x29ae18));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_148)) = (*(uint32_t*)rtguest::G(0x29ae1c));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_144)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae14))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_14c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_150)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_144 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_148)));
  RT_CALLRA(0x257294, guest::sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_110)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = (*(uint32_t*)rtguest::G(0x29ae70));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = (*(uint32_t*)rtguest::G(0x29ae6c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 1;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_104)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae68))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_10c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_110)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_104 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_108)));
  RT_CALLRA(0x257330, guest::sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_d0)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = (*(uint32_t*)rtguest::G(0x29ae78));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = (*(uint32_t*)rtguest::G(0x29ae7c));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_c4)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae74))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_cc)),(*(uint32_t*)rtguest::G((uint32_t)uStack_d0)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_c4 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_c8)));
  RT_CALLRA(0x2573cc, guest::sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0));
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_90)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_90)) = 2;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_8c)) = (*(uint32_t*)rtguest::G(0x29ae6c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G(0x29ae70));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)uStack_84)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae68))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_8c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_90)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_84)),(*(uint32_t*)rtguest::G((uint32_t)uStack_88)));
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_80));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_78));
  (*(uint64_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_70));
  (*(uint64_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_68));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  RT_CALLRA(0x257468, guest::sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0));
  return 0;
}


RT_GUEST_FN(0x00256af8, guest::InitSoundRpcDriver)

}  // namespace guest
