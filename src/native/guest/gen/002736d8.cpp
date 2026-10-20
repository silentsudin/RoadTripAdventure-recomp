// guest:0x002736d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadInit2(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
uint32_t puVar1;
  undefined4 uVar2;
  long lVar3;
  int iVar4;
  
  puVar1 = 0x01773294;
  iVar4 = 3;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(-0x1d) * sizeof(uint32_t))) = 0;
    iVar4 = iVar4 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(-0x1b) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(-0x1c) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(-1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 0;
    puVar1 = puVar1 + 0x1c;  // T* element step
  } while (-1 < iVar4);
  (*(uint32_t*)rtguest::G(0x1773500)) = 0x10;
  (*(uint32_t*)rtguest::G(0x1773510)) = 0;
  lVar3 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
  uVar2 = (*(uint32_t*)rtguest::G(0x177350c));
  if (lVar3 < 0) {
    uVar2 = 0;
  }
  return uVar2;
}


RT_GUEST_FN(0x002736d8, guest::scePadInit2)

}  // namespace guest
