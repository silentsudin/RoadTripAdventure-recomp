// guest:0x0028cbd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSetIopAddr(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x40) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x44) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  uint32_t uVar1;
  uint32_t lVar2;
  
  lVar2 = RT_CALLRA(0x28cbec, guest::_lf_bind());
  if ((int32_t)lVar2 < 0) {
    uVar1 = 0xffff0000;
  }
  else {
    *(uint32_t*)rtguest::G(0x1779e04) = param_3;  // DAT_01779e04
    if (param_3 == 0) {
      *(uint8_t*)rtguest::G(0x1779e08) = (uint8_t)*(uint8_t*)rtguest::G((uint32_t)param_2);  // DAT_01779e08 low byte (CONCAT31 merge)
    }
    else if (param_3 == 1) {
      *(uint16_t*)rtguest::G(0x1779e08) = *(uint16_t*)rtguest::G((uint32_t)param_2);  // CONCAT22 merge
    }
    else {
      if (param_3 != 2) {
        *(uint32_t*)rtguest::G(0x1779e00) = param_1;  // DAT_01779e00
        return 0xfffefffe;
      }
      *(uint32_t*)rtguest::G(0x1779e08) = *(uint32_t*)rtguest::G((uint32_t)param_2);
    }
    *(uint32_t*)rtguest::G(0x1779e00) = param_1;
    lVar2 = RT_CALLRA(0x28cc74, guest::sceSifCallRpc(0x177a000,2,0,0x1779e00,0x20,0x1779e00,0x10,0));
    uVar1 = 0xfffeffff;
    if ((int32_t)lVar2 >= 0) {  // pseudocode: -1 < lVar2
      uVar1 = 0;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028cbd0, guest::sceSifSetIopAddr)

}  // namespace guest
