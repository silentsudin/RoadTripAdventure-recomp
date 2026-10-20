// guest:0x0020dde0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t loadFontTextures(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  // 2026-10-19 MIPS cross-check (0x20dde0-0x20de94): Ghidra's pseudocode
  // halved the chunk walk (qword +0x10 / stride size*8+8). Real MIPS:
  // ld/sd 0x20($s0); addiu $s0,$a0,0x10 with $a0=p+size<<4 → stride
  // size*16+0x10; type byte lbu 3($s0). Verified against SYS/FONTE.GSL:
  // correct walk ends at 'p' after 2 chunks; Ghidra's walk hits the game's
  // own fatal "GSL DMA Tag Error!" (0x2ea490).
  char cVar1;
  uint32_t puVar2;
  uint64_t uVar3;
  uint32_t size;
  undefined8 uVar4;
  
  cdReadEntryWait(0x29af58,0x100000);
  puVar2 = 0x00100000;
  cVar1 = (*(int8_t*)rtguest::G(0x100003));
  while (cVar1 != 'p') {
    if (cVar1 != '\x10') {
      fatal(0x2ea490);
    }
    size = (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar2));
    uVar3 = (*(uint64_t*)rtguest::G(puVar2 + 0x20));
    (*(uint64_t*)rtguest::G(puVar2 + 0x20)) =
         uVar3 & 0xffffc000ffffffffull |
         (uint64_t)((((uint32_t)(uVar3 >> 0x20) & 0x3fff) + 0x1a40) & 0x3fffu) << 0x20;
    puVar2 = puVar2 + size * 0x10 + 0x10;
    cVar1 = (*(int8_t*)rtguest::G(puVar2 + 3));
  }
  uVar4 = sceDmaGetChan(1);
  sceDmaSync(uVar4,0,0);
  sendTexturePackets(0x100000);
  return 0;
}


RT_GUEST_FN(0x0020dde0, guest::loadFontTextures)

}  // namespace guest
