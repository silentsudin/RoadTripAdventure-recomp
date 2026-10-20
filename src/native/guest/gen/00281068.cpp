// guest:0x00281068 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vsprintf(uint32_t __s, uint32_t __format, uint32_t __arg, ...) {
  rtguest::FrameGuard rt_fg(112);
  uint32_t apcStack_70 = rtguest::allocFrame(8);  // sp+0x0 (Ghidra entry-sp -0x70, frame 0x70)
  rtguest::allocFrame(4);  // guest frame gap (sp+0x8..0xb)
  uint32_t uStack_64 = rtguest::allocFrame(2);  // sp+0xc (Ghidra entry-sp -0x64, frame 0x70)
  rtguest::allocFrame(2);  // guest frame gap (sp+0xe..0xf)
  uint32_t pcStack_60 = rtguest::allocFrame(4);  // sp+0x10 (Ghidra entry-sp -0x60, frame 0x70)
  uint32_t uStack_5c = rtguest::allocFrame(4);  // sp+0x14 (Ghidra entry-sp -0x5c, frame 0x70)
  rtguest::allocFrame(60);  // guest frame gap (sp+0x18..0x53)
  uint32_t puStack_1c = rtguest::allocFrame(4);  // sp+0x54 (Ghidra entry-sp -0x1c, frame 0x70)
  int iVar1;
  
  *(uint32_t*)rtguest::G((uint32_t)uStack_5c) = 0x7fffffff;
  *(uint32_t*)rtguest::G((uint32_t)(apcStack_70 + 4)) = 0x7fffffff;  // uStack_68
  *(uint16_t*)rtguest::G((uint32_t)uStack_64) = 0x208;
  *(uint32_t*)rtguest::G((uint32_t)puStack_1c) = *(uint32_t*)rtguest::G(0x2e8cc0);  // PTR_DAT_002e8cc0
  *(uint32_t*)rtguest::G((uint32_t)apcStack_70) = __s;
  *(uint32_t*)rtguest::G((uint32_t)pcStack_60) = __s;
  iVar1 = RT_CALLRA(0x2810a0, guest::vfprintf(apcStack_70,__format,__arg));
  *(uint8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apcStack_70))) = 0;
  return (uint32_t)iVar1;
}


RT_GUEST_FN(0x00281068, guest::vsprintf)

}  // namespace guest
