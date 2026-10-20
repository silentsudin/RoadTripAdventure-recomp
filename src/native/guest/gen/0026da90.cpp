// guest:0x0026da90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t PrintDoorBase(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  undefined8 uVar1;
uint32_t auStack_40 = rtguest::allocFrame(32);
  
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335630)), param_2);
  RT_CALLRA(0x26dac4, guest::uiDebugText8(0x10,0x20,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26dad0, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335638)), uVar1);
  RT_CALLRA(0x26db00, guest::uiDebugText8(0x10,0x30,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26db08, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335640)), uVar1);
  RT_CALLRA(0x26db30, guest::uiDebugText8(0x10,0x38,auStack_40,0xf));
  uVar1 = RT_CALLRA(0x26db3c, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)))));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_40)), (char *)rtguest::G((uint32_t)(0x335648)), uVar1);
  RT_CALLRA(0x26db60, guest::uiDebugText8(0x10,0x40,auStack_40,0xf));
  return 0;
}


RT_GUEST_FN(0x0026da90, guest::PrintDoorBase)

}  // namespace guest
