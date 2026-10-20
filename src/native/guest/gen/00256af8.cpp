// guest:0x00256af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitSoundRpcDriver(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(1120);
    uint32_t uStack_250 = rtguest::allocFrame(4);
    uint32_t uStack_24c = rtguest::allocFrame(4);
    uint32_t uStack_248 = rtguest::allocFrame(2);
    uint32_t uStack_246 = rtguest::allocFrame(2);
    uint32_t uStack_244 = rtguest::allocFrame(4);
    uint32_t uStack_240 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_230 = rtguest::allocFrame(4);
    uint32_t uStack_22c = rtguest::allocFrame(4);
    uint32_t uStack_228 = rtguest::allocFrame(2);
    uint32_t uStack_226 = rtguest::allocFrame(2);
    uint32_t uStack_224 = rtguest::allocFrame(4);
    uint32_t uStack_220 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_210 = rtguest::allocFrame(4);
    uint32_t uStack_20c = rtguest::allocFrame(4);
    uint32_t uStack_208 = rtguest::allocFrame(4);
uint32_t auStack_204 = rtguest::allocFrame(52);
    uint32_t uStack_1d0 = rtguest::allocFrame(4);
    uint32_t uStack_1cc = rtguest::allocFrame(4);
    uint32_t uStack_1c8 = rtguest::allocFrame(4);
uint32_t auStack_1c4 = rtguest::allocFrame(52);
    uint32_t uStack_190 = rtguest::allocFrame(4);
    uint32_t uStack_18c = rtguest::allocFrame(4);
    uint32_t uStack_188 = rtguest::allocFrame(4);
uint32_t auStack_184 = rtguest::allocFrame(52);
    uint32_t uStack_150 = rtguest::allocFrame(4);
    uint32_t uStack_14c = rtguest::allocFrame(4);
    uint32_t uStack_148 = rtguest::allocFrame(4);
uint32_t auStack_144 = rtguest::allocFrame(52);
    uint32_t uStack_110 = rtguest::allocFrame(4);
    uint32_t uStack_10c = rtguest::allocFrame(4);
    uint32_t uStack_108 = rtguest::allocFrame(4);
uint32_t auStack_104 = rtguest::allocFrame(52);
    uint32_t uStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    uint32_t uStack_c8 = rtguest::allocFrame(4);
uint32_t auStack_c4 = rtguest::allocFrame(52);
    uint32_t uStack_90 = rtguest::allocFrame(4);
    uint32_t uStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    uint32_t uStack_84 = rtguest::allocFrame(4);
    uint32_t uStack_80 = rtguest::allocFrame(8);
    uint32_t uStack_78 = rtguest::allocFrame(8);
    uint32_t uStack_70 = rtguest::allocFrame(8);
    uint32_t uStack_68 = rtguest::allocFrame(8);
    uint32_t uStack_60 = rtguest::allocFrame(8);
    uint32_t uStack_58 = rtguest::allocFrame(8);
  
  sceSdRemoteInit();
  sceSifBindRpc(0x175e840,0x1234567,0);
  sceSifCallRpc(0x175e840,0,0,0,0,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0;
  sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x1000000000001;
  sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x2000000000002;
  sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x3000000000003;
  sceSifCallRpc(0x175e840,5,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x1b000000000000;
  sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x16000000000001;
  sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x16000000000002;
  sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0);
  (*(uint64_t*)rtguest::G(0x175e880)) = 0x10500000000003;
  sceSifCallRpc(0x175e840,4,0,0x175e880,0x40,0,0,0);
  sceSdRemote(1,0x8050,0x1d00,0x15ffff);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_24c)) = 0x103;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_248)) = 0x3fff;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_246)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_244)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_240)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_250)) = 0;
  sceSdRemote(1,0x8130,0,uStack_250);
  sceSdRemote(1,0x8070,2,1);
  sceSdRemote(1,0x8050,0x1d01,0x1fffff);
  (*(uint32_t*)rtguest::G((uint32_t)uStack_22c)) = 0x103;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_226)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_220)) = 0x3f;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_230)) = 1;
  (*(uint16_t*)rtguest::G((uint32_t)uStack_228)) = 0x3fff;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_224)) = 0x3f;
  sceSdRemote(1,0x8130,1,uStack_230);
  sceSdRemote(1,0x8070,3,1);
  SoundRpcRegisterFiles(0,0x29ae20,0x29ae2c);
  SoundRpcRegisterFiles(2,0x29ae38,0x29ae44);
  SoundRpcRegisterFiles(1,0x29ae50,0x29ae5c);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_210)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_20c)) = (*(uint32_t*)rtguest::G(0x29adf4));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_208)) = (*(uint32_t*)rtguest::G(0x29adf8));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_210)) = 0;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_204)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x29adf0))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_20c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_210)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_204 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_208)));
  sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_1d0)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1cc)) = (*(uint32_t*)rtguest::G(0x29ae00));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1c8)) = (*(uint32_t*)rtguest::G(0x29ae04));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1d0)) = 0;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_1c4)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029adfc))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_1cc)),(*(uint32_t*)rtguest::G((uint32_t)uStack_1d0)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_1c4 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_1c8)));
  sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_190)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_188)) = (*(uint32_t*)rtguest::G(0x29ae10));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18c)) = (*(uint32_t*)rtguest::G(0x29ae0c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_190)) = 3;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_184)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae08))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_18c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_190)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_184 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_188)));
  sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_150)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_150)) = 3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_14c)) = (*(uint32_t*)rtguest::G(0x29ae18));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_148)) = (*(uint32_t*)rtguest::G(0x29ae1c));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_144)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae14))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_14c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_150)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_144 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_148)));
  sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_110)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = (*(uint32_t*)rtguest::G(0x29ae70));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_10c)) = (*(uint32_t*)rtguest::G(0x29ae6c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_110)) = 1;
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_104)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae68))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_10c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_110)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_104 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_108)));
  sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_d0)), 0, 0x40));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_d0)) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = (*(uint32_t*)rtguest::G(0x29ae78));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_c8)) = (*(uint32_t*)rtguest::G(0x29ae7c));
  (uint32_t)(uintptr_t)(::strcpy((char *)rtguest::G((uint32_t)((uint32_t)auStack_c4)), (char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G(0x0029ae74))))));
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_cc)),(*(uint32_t*)rtguest::G((uint32_t)uStack_d0)));
  (*(uint64_t*)rtguest::G(0x175e888)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)auStack_c4 + (uint32_t)(0) * 4)),(*(uint32_t*)rtguest::G((uint32_t)uStack_c8)));
  sceSifCallRpc(0x175e840,7,0,0x175e880,0x40,0,0,0);
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
  sceSifCallRpc(0x175e840,6,0,0x175e880,0x40,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00256af8, guest::InitSoundRpcDriver)

}  // namespace guest
