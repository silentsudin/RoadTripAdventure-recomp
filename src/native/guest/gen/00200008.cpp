// guest:0x00200008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t entry(uint32_t q0, ...) {
  bool bVar1;
  undefined4 in_zero_lo;
  undefined4 in_zero_hi;
  undefined4 in_zero_udw;
  undefined4 in_register_0000000c;
uint32_t puVar2;
  int __status;
  
  puVar2 = 0x335680;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = in_zero_lo;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) = in_zero_hi;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) = in_zero_udw;
    (*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint32_t))) = in_register_0000000c;
    bVar1 = puVar2 < 0x017874e0;
    puVar2 = puVar2 + 0x10;  // T* element step
  } while (bVar1);
  syscall(0x3c);
  syscall(0x3d);
  _InitSys(0x17874e0,0xffffffffffffffff,0x80000,0x335780,0x2000c0);
  FlushCache(0);
  EI();
  __status = gameMainLoop((*(uint32_t*)rtguest::G(0x335780)),0x335784);
  _Exit(__status);
  return 0;
}


RT_GUEST_FN(0x00200008, guest::entry)

}  // namespace guest
