// guest:0x00204d60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DestroyTaskRecord(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
uint32_t puVar1;
  int iVar2;
  
  puVar1 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  if (puVar1 != (uint32_t)0x0) {
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x2c))) == (uint32_t)0x0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10)));
    }
    else {
      rtCall((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x2c))), puVar1);  // MIPS 0x204d80/d8c: lw $v0,0x2c($s0); jalr — direct handler ptr, Ghidra (**code**) mistranslation (cf. 0x205028)
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10)));
    }
    (*(uint8_t*)rtguest::G((uint32_t)puVar1)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x28))) = 0;
    if (iVar2 != 0) {
      RT_CALLRA(0x204da8, guest::DestroyTaskTree(iVar2));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x10))) = 0;
    }
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14))) != 0) {
      RT_CALLRA(0x204dc0, guest::DestroyTaskTree((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14)))));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar1 + 0x14))) = 0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00204d60, guest::DestroyTaskRecord)

}  // namespace guest
