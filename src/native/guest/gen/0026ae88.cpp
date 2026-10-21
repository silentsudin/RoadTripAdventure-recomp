// guest:0x0026ae88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyAdjusterScales(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x48, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x50, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x58) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x5c) = 0x00000000u;
  }
  ushort uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c)));
  if ((uVar1 & 2) != 0) {
    uVar2 = RT_CALLRA(0x26aeb4, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c)))));
    uVar2 = RT_CALLRA(0x26aec4, guest::dpadd(uVar2,0x3ff0000000000000));
    uVar3 = RT_CALLRA(0x26aed0, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))) = uVar3;
    uVar2 = RT_CALLRA(0x26aedc, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)))));
    uVar2 = RT_CALLRA(0x26aeec, guest::dpadd(uVar2,0x3ff0000000000000));
    uVar3 = RT_CALLRA(0x26aef8, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))) = uVar3;
    uVar2 = RT_CALLRA(0x26af04, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x16c)))));
    uVar2 = RT_CALLRA(0x26af18, guest::dpdiv(0x4079000000000000,uVar2));
    uVar3 = RT_CALLRA(0x26af24, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x158))) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x38))) = uVar3;
  }
  if ((uVar1 & 8) == 0) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)));
  }
  else {
    uVar2 = RT_CALLRA(0x26af48, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c)))));
    uVar2 = RT_CALLRA(0x26af58, guest::dpsub(uVar2,0x3ff0000000000000));
    uVar3 = RT_CALLRA(0x26af64, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))) = uVar3;
    uVar2 = RT_CALLRA(0x26af70, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)))));
    uVar2 = RT_CALLRA(0x26af80, guest::dpsub(uVar2,0x3ff0000000000000));
    uVar3 = RT_CALLRA(0x26af8c, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))) = uVar3;
    uVar2 = RT_CALLRA(0x26af98, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x16c)))));
    uVar2 = RT_CALLRA(0x26afac, guest::dpdiv(0x4079000000000000,uVar2));
    uVar3 = RT_CALLRA(0x26afb8, guest::dptofp(uVar2));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x158))) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x38))) = uVar3;
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)));
  }
  uVar2 = RT_CALLRA(0x26afd0, guest::fptodp(uVar3));
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32f8c0)), uVar2, (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x170))));
  RT_CALLRA(0x26aff8, guest::uiDebugText8(0x10,0x80,auStack_60,0xf));
  return 0;
}


RT_GUEST_FN(0x0026ae88, guest::ApplyAdjusterScales)

}  // namespace guest
