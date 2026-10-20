// guest:0x00200008 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t entry(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
  // W12 fix: the bss-clear loop stores $zero four times per word (disasm
  // 0x200018 dext $zero + sw $zero x4); the Ghidra "in_zero" locals must be 0.
  undefined4 in_zero_lo = 0;
  undefined4 in_zero_hi = 0;
  undefined4 in_zero_udw = 0;
  undefined4 in_register_0000000c = 0;
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
  // W12: register args per disasm 0x200034-0x200064: SetupThread($4 gp=0x33AAF0,
  // $5 stack=0x1F80000, $6 size=0x80000; $t0 root_func 0x2000C0 — the oracle
  // model ignores it) then SetupHeap($4=0x17874E0 end-of-bss, $5=-1 rest of RAM).
  syscall(0x3c, 0x33AAF0, 0x1F80000, 0x80000);
  syscall(0x3d, 0x17874E0, 0xFFFFFFFF);
  _InitSys(0x17874e0,0xffffffffffffffff,0x80000,0x335780,0x2000c0);
  FlushCache(0);
  EI();
  __status = gameMainLoop((*(uint32_t*)rtguest::G(0x335780)),0x335784);
  _Exit(__status);
  return 0;
}


RT_GUEST_FN(0x00200008, guest::entry)

}  // namespace guest
