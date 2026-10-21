// guest:0x002862c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyWrite(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x60) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x64) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[3] = 0x0000000001776a90ull;  // walk const
    rtguest::gS[4] = 0x0000000000000000ull;  // walk const
    rtguest::gS[5] = 0x0000000001770000ull;  // walk const
  }
  long lVar1;
  char cVar2;
uint32_t pcVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = 0;
  iVar5 = -1;
  if ((*(int32_t*)rtguest::G(0x1776a9c)) == 0) {
    RT_CALLRA(0x28630c, guest::DIntr());
    (*(int32_t*)rtguest::G(0x1776a9c)) = 1;
    (*(uint32_t*)rtguest::G(0x1776aa0)) = 0x21776ac0;
    pcVar3 = 0x21776acc;
    iVar5 = 0;
    do {
      param_2 = param_2 + -1;
      if (param_2 == -1) break;
      cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == '\n') {
        (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = '\r';
        iVar4 = iVar4 + 1;
        pcVar3 = pcVar3 + 1;
        if (0xff < iVar4) break;
        cVar2 = (*(int8_t*)rtguest::G((uint32_t)param_1));
      }
      (*(int8_t*)rtguest::G((uint32_t)pcVar3)) = cVar2;
      iVar4 = iVar4 + 1;
      param_1 = param_1 + 1;
      pcVar3 = pcVar3 + 1;
      iVar5 = iVar5 + 1;
    } while (iVar4 < 0x100);
    (*(uint32_t*)rtguest::G(0x1776a94)) = iVar4 + 0xc;
    (*(uint32_t*)rtguest::G(0x21776ac0)) = (uint32_t)(*(uint32_t*)rtguest::G(0x1776a94));
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
    lVar1 = RT_CALLRA(0x2863a4, guest::sceDeci2ReqSend((*(int32_t*)rtguest::G(0x1776a90)),(*(uint32_t*)rtguest::G(0x21776ac7))));
    if (lVar1 < 0) {
      (*(int32_t*)rtguest::G(0x1776a9c)) = 0;
      RT_CALLRA(0x2863b4, guest::EIntr());
      iVar5 = -1;
    }
    else {
      while ((*(int32_t*)rtguest::G(0x1776a9c)) != 0) {
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
        RT_CALLRA(0x2863d4, guest::sceDeci2Poll((*(int32_t*)rtguest::G(0x1776a90))));
      }
                    /* WARNING: Read-only address (ram,0x21776ac0) is written */
      RT_CALLRA(0x2863e8, guest::EIntr());
    }
  }
  return iVar5;
}


RT_GUEST_FN(0x002862c8, guest::sceTtyWrite)

}  // namespace guest
