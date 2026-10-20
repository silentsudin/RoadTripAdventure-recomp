// guest:0x0020e030 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 2026-10-19 MIPS cross-check 0x0020e030: Ghidra halved chunk walk (qword +0x10 / stride size*8+8); real MIPS qword +0x20, stride size*16+0x10 (see gen/0020dde0.cpp).
uint32_t SendTexturePacketsDMA(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
  }
  char cVar1;
uint32_t puVar2;
  undefined8 uVar3;
  ulong uVar4;
  
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 3)));
  puVar2 = param_1;
  while (cVar1 != 'p') {
    if (cVar1 == '\x10') {
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20)));
    }
    else {
      RT_CALLRA(0x20e0a0, guest::fatal(0x2ea490));
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20)));
    }
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x20))) =
         uVar4 & 0xffffc000ffffffff |
         (long)(int)(((uint32_t)(uVar4 >> 0x20) & 0x3fff) + (int)param_2 & 0x3fff) << 0x20;
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x13)));
    puVar2 = puVar2 + (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2)) * 0x10 + 0x10;
  }
  uVar3 = RT_CALLRA(0x20e0f0, guest::sceDmaGetChan(1));
  RT_CALLRA(0x20e100, guest::sceDmaSync(uVar3,0,0));
  RT_CALLRA(0x20e108, guest::sendTexturePackets(param_1));
  return 0;
}


RT_GUEST_FN(0x0020e030, guest::SendTexturePacketsDMA)

}  // namespace guest
