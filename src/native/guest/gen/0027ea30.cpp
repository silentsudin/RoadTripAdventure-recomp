// guest:0x0027ea30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t printf(uint32_t __format, ...) {
  rtguest::FrameGuard rt_fg(112);
    rtguest::allocFrame(56);  // guest frame gap (sp+0x0..0x37)
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
    uint32_t uStack_38 = rtguest::allocFrame(8);  // sp+0x38 (Ghidra entry-sp -0x38, frame 0x70)
    uint32_t uStack_30 = rtguest::allocFrame(8);  // sp+0x40 (Ghidra entry-sp -0x30, frame 0x70)
    uint32_t uStack_28 = rtguest::allocFrame(8);  // sp+0x48 (Ghidra entry-sp -0x28, frame 0x70)
    uint32_t uStack_20 = rtguest::allocFrame(8);  // sp+0x50 (Ghidra entry-sp -0x20, frame 0x70)
    uint32_t uStack_18 = rtguest::allocFrame(8);  // sp+0x58 (Ghidra entry-sp -0x18, frame 0x70)
    uint32_t uStack_10 = rtguest::allocFrame(8);  // sp+0x60 (Ghidra entry-sp -0x10, frame 0x70)
    uint32_t uStack_8 = rtguest::allocFrame(8);  // sp+0x68 (Ghidra entry-sp -0x8, frame 0x70)
uint32_t puVar1;
  int iVar2;
  undefined8 in_a1;
  undefined8 in_a2;
  undefined8 in_a3;
  undefined8 in_t0;
  undefined8 in_t1;
  undefined8 in_t2;
  undefined8 in_t3;
  
  puVar1 = (*(uint32_t*)rtguest::G(0x002e8cc0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002e8cc0)) + 8))) + 0x54))) = (*(uint32_t*)rtguest::G(0x002e8cc0));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_38)) = in_a1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = in_a2;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = in_a3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = in_t0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = in_t1;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = in_t2;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = in_t3;
  iVar2 = RT_CALLRA(0x27ea8c, guest::vfprintf((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 8))),__format,uStack_38));
  return iVar2;
}


RT_GUEST_FN(0x0027ea30, guest::printf)

}  // namespace guest
