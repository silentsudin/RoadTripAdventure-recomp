// guest:0x00257ba8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SoundRpcGetStatus(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(128);
    rtguest::allocFrame(20);  // guest frame gap (sp+0x0..0x13)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = 0x00000001u;
  }
    uint32_t uStack_6c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x6c, frame 0x80)
    uint32_t uStack_68 = rtguest::allocFrame(8);  // sp+0x18 (Ghidra entry-sp -0x68, frame 0x80)
    uint32_t uStack_60 = rtguest::allocFrame(8);  // sp+0x20 (Ghidra entry-sp -0x60, frame 0x80)
    uint32_t uStack_58 = rtguest::allocFrame(8);  // sp+0x28 (Ghidra entry-sp -0x58, frame 0x80)
    uint32_t uStack_50 = rtguest::allocFrame(8);  // sp+0x30 (Ghidra entry-sp -0x50, frame 0x80)
    uint32_t uStack_48 = rtguest::allocFrame(8);  // sp+0x38 (Ghidra entry-sp -0x48, frame 0x80)
    uint32_t uStack_40 = rtguest::allocFrame(8);  // sp+0x40 (Ghidra entry-sp -0x40, frame 0x80)
    uint32_t uStack_38 = rtguest::allocFrame(8);  // sp+0x48 (Ghidra entry-sp -0x38, frame 0x80)
  
  (*(uint64_t*)rtguest::G(0x175e880)) = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),1);
  (*(uint64_t*)rtguest::G(0x175e888)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_68));
  (*(uint64_t*)rtguest::G(0x175e890)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_60));
  (*(uint64_t*)rtguest::G(0x175e898)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_58));
  (*(uint32_t*)rtguest::G(0x175e8a0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_50));
  (*(uint32_t*)rtguest::G(0x175e8a8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_48));
  (*(uint64_t*)rtguest::G(0x175e8b0)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_40));
  (*(uint32_t*)rtguest::G(0x175e8b8)) = (*(uint64_t*)rtguest::G((uint32_t)uStack_38));
  RT_CALLRA(0x257c3c, guest::sceSifCallRpc(0x175e840,8,0,0x175e880,0x40,0x175f880,0x40,0));
  return (uint32_t)(*(uint64_t*)rtguest::G(0x175f880));
}


RT_GUEST_FN(0x00257ba8, guest::SoundRpcGetStatus)

}  // namespace guest
