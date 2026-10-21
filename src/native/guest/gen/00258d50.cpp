// guest:0x00258d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SndRpcReadStatBlock(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::gS[0] = 0x000000000175f880ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
uint32_t puVar4;
  
  puVar4 = 0x0175f880;
  RT_CALLRA(0x258d90, guest::sceSifCallRpc(0x175e840,0xfff0,0,0,0,0x175f880,0x300,0));
  if (((uint32_t)param_1 & 7) == 0) {
    do {
      uVar1 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t)));
      uVar2 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t)));
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)puVar4));
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar1;
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar2;
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar3;
      puVar4 = puVar4 + (uint32_t)(4) * sizeof(uint64_t);
      param_1 = param_1 + (uint32_t)(4) * sizeof(uint64_t);
    } while (puVar4 != (uint32_t)0x0175fb80);
  }
  else {
    do {
      uVar1 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(1) * sizeof(uint64_t)));
      uVar2 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(2) * sizeof(uint64_t)));
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(3) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G((uint32_t)puVar4));
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar1;
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar2;
      (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar3;
      puVar4 = puVar4 + (uint32_t)(4) * sizeof(uint64_t);
      param_1 = param_1 + (uint32_t)(4) * sizeof(uint64_t);
    } while (puVar4 != (uint32_t)0x0175fb80);
  }
  return 0;
}


RT_GUEST_FN(0x00258d50, guest::SndRpcReadStatBlock)

}  // namespace guest
