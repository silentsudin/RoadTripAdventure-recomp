// guest:0x00272e08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t vu0NormalLightMatrix(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x50);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(2, (uint32_t)param_4);  // walk: = param_4
  }
  int iVar1;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  // MIPS 0x272e38 jal vu0ScaleVector: $f12=-1 (0x272e30), $4=sp (delay 0x272e3c), $5 = param_2 untouched since entry
  // (Ghidra dropped the live $a1); same shape as the two calls below.
  RT_CALLRA(0x272e38, guest::vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60,param_2));
  RT_CALLRA(0x272e44, guest::vecNormalizeVu0(param_1,auStack_60));
  RT_CALLRA(0x272e54, guest::vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60,param_3));
  iVar1 = (int)param_1;
  RT_CALLRA(0x272e60, guest::vecNormalizeVu0(iVar1 + 0x10,auStack_60));
  RT_CALLRA(0x272e70, guest::vu0ScaleVector(rtguest::fbits(0xbf800000),auStack_60,param_4));
  RT_CALLRA(0x272e7c, guest::vecNormalizeVu0(iVar1 + 0x20,auStack_60));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x30))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x3c))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x34))) = 0;
  RT_CALLRA(0x272ea4, guest::mat4Transpose(param_1,param_1));
  return 0;
}


RT_GUEST_FN(0x00272e08, guest::vu0NormalLightMatrix)

}  // namespace guest
