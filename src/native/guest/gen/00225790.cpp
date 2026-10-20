// guest:0x00225790 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateSceneLightsAndFog(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
  }
uint32_t puVar1;
uint32_t pcVar2;
  int iVar3;
  
  (*(uint32_t*)rtguest::G(0x177f740)) = 0x43000000;
  (*(uint32_t*)rtguest::G(0x177f720)) = 0x3f800000;
  (*(uint32_t*)rtguest::G(0x177f744)) = 0x437f0000;
  (*(uint32_t*)rtguest::G(0x177f748)) = 0x3f000000;
  (*(uint32_t*)rtguest::G(0x177f74c)) = 0x44480000;
  (*(uint32_t*)rtguest::G(0x177f724)) = 0;
  (*(uint32_t*)rtguest::G(0x177f728)) = 0;
  (*(uint32_t*)rtguest::G(0x177f730)) = 0x43000000;
  (*(uint32_t*)rtguest::G(0x177f734)) = 0x437f0000;
  (*(uint32_t*)rtguest::G(0x177f738)) = 0x3f000000;
  (*(uint32_t*)rtguest::G(0x177f73c)) = 0x44480000;
  if (param_1 == 1) {
    puVar1 = 0x0029e950;
  }
  else if (param_1 == 2) {
    puVar1 = 0x0029e980;
  }
  else if (param_1 == 3) {
    puVar1 = 0x0029e9e0;
  }
  else {
    if (param_2 != -1) {
      iVar3 = (int)param_2 * 0x30;
      RT_CALLRA(0x225878, guest::vu0NormalLightMatrix(0x177f5e0,0x0029e200 + iVar3,0x0029e210 + iVar3,0x0029e220 + iVar3));
      goto LAB_002258b8;
    }
    puVar1 = 0x0029e200;
  }
  RT_CALLRA(0x2258ac, guest::vu0NormalLightMatrix(0x177f5e0,puVar1,puVar1 + 0x10,puVar1 + 0x20));
LAB_002258b8:
  (*(uint32_t*)rtguest::G(0x177f610)) = 0x43000000;
  (*(uint32_t*)rtguest::G(0x177f614)) = 0x437f0000;
  iVar3 = 0x27;
  (*(uint32_t*)rtguest::G(0x177f618)) = 0x3f000000;
  (*(uint32_t*)rtguest::G(0x177f61c)) = 0x44480000;
  RT_CALLRA(0x2258f8, guest::vu0CopyMatrix(0x177f620,0x29d500));
  RT_CALLRA(0x225904, guest::vu0CopyMatrix(0x177f660,0x29d5c0));
  pcVar2 = (uint32_t)0x29ec92;
  do {
    if (param_2 == (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(-2) * sizeof(int8_t)))) {
      RT_CALLRA(0x225930, guest::vu0CopyMatrix(0x177f6a0,0x29d500 + (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(-1) * sizeof(int8_t))) * 0x40));
      RT_CALLRA(0x225944, guest::vu0CopyMatrix(0x177f6e0,0x29d500 + (*(int8_t*)rtguest::G((uint32_t)pcVar2)) * 0x40));
    }
    iVar3 = iVar3 + -1;
    pcVar2 = pcVar2 + 3;
  } while (-1 < iVar3);
  (*(uint32_t*)rtguest::G(0x177f750)) = 0x80ffffff;
  return 0;
}


RT_GUEST_FN(0x00225790, guest::UpdateSceneLightsAndFog)

}  // namespace guest
