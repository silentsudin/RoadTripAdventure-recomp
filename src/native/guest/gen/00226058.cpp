// guest:0x00226058 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ScaleCarLightMatrix(uint32_t rt_bits_1, uint32_t param_2, ...) {
  float param_1 = rtguest::fbits(rt_bits_1);  // f12.. float arg carried as bits (W13 float-param fix)
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
  }
uint32_t auStack_70 = rtguest::allocFrame(16);
uint32_t auStack_60 = rtguest::allocFrame(16);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(12);
    uint32_t uStack_34 = rtguest::allocFrame(4);
    uint32_t fStack_30 = rtguest::allocFrame(4);
    uint32_t fStack_2c = rtguest::allocFrame(4);
    uint32_t fStack_28 = rtguest::allocFrame(4);
  int iVar1;
  
  if (-2.0 < param_1) {
    param_1 = 0.0;
    (*(float*)rtguest::G((uint32_t)fStack_30)) = 1.0;
  }
  else {
    (*(float*)rtguest::G((uint32_t)fStack_30)) = param_1 * (*(float*)rtguest::G(0x332dcc)) + 0.5;
  }
  (*(float*)rtguest::G((uint32_t)fStack_28)) = param_1 * (*(float*)rtguest::G(0x332dd0)) + 1.0;
  if ((*(float*)rtguest::G((uint32_t)fStack_30)) < 0.0) {
    (*(float*)rtguest::G((uint32_t)fStack_30)) = 0.0;
  }
  if ((*(float*)rtguest::G((uint32_t)fStack_28)) < 0.25) {
    (*(float*)rtguest::G((uint32_t)fStack_28)) = 0.25;
  }
  (*(float*)rtguest::G((uint32_t)fStack_2c)) = (*(float*)rtguest::G((uint32_t)fStack_28));
  RT_CALLRA(0x22611c, guest::vu0MulVector(auStack_70,param_2,fStack_30));
  iVar1 = (int)param_2;
  RT_CALLRA(0x22612c, guest::vu0MulVector(auStack_60,iVar1 + 0x10,fStack_30));
  RT_CALLRA(0x22613c, guest::vu0MulVector(auStack_50,iVar1 + 0x20,fStack_30));
  RT_CALLRA(0x22614c, guest::vu0MulVector(auStack_40,iVar1 + 0x30,fStack_30));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_34)) = 0x43000000;
  RT_CALLRA(0x226160, guest::vifUploadLightMatrix(auStack_70));
  return 0;
}


RT_GUEST_FN(0x00226058, guest::ScaleCarLightMatrix)

}  // namespace guest
