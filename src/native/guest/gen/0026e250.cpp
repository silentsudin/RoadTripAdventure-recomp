// guest:0x0026e250 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsResetGraph(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
  }
  undefined8 uVar1;
uint32_t puVar2;
  
  if (param_1 == 1) {
    (*(uint8_t*)rtguest::G(0x12001000)) = 0x100;
  }
  else if (param_1 < 2) {
    if (param_1 == 0) {
      puVar2 = (uint32_t)RT_CALLRA(0x26e2bc, guest::sceGsGetEnv());
      (*(uint8_t*)rtguest::G(0x12001000)) = 0x200;
      (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = param_2;
      uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint16_t))) = param_3;
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint16_t))) = (uint32_t)((uint32_t)uVar1 >> 0x10) & 0xff;
      RT_CALLRA(0x26e2f4, guest::GsPutIMR(0xff00));
      (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint16_t))) = (uint32_t)(param_4 != 0);
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 4))) != 0) {
        RT_CALLRA(0x26e30c, guest::disableIntc(2));
        RT_CALLRA(0x26e318, guest::RemoveIntcHandler(2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 6)))));
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(6) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(7) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(4) * sizeof(uint16_t))) = 0;
        (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(5) * sizeof(uint16_t))) = 0;
      }
      SetGsCrt(param_2 & 1,param_3 & 0xff,param_4 & 1);
      return 0;
    }
  }
  else if (param_1 == 5) {
    puVar2 = (uint32_t)RT_CALLRA(0x26e368, guest::sceGsGetEnv());
    uVar1 = (*(uint8_t*)rtguest::G(0x12001000));
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint16_t))) = (uint32_t)(param_4 != 0);
    (*(uint16_t*)rtguest::G((uint32_t)puVar2)) = param_2;
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint16_t))) = param_3;
    (*(uint16_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(3) * sizeof(uint16_t))) = (uint32_t)((uint32_t)uVar1 >> 0x10) & 0xff;
    SetGsCrt(param_2 & 1,param_3 & 0xff,param_4 & 1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026e250, guest::sceGsResetGraph)

}  // namespace guest
