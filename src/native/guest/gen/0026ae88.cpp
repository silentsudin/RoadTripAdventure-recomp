// guest:0x0026ae88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ApplyAdjusterScales(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  ushort uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
uint32_t auStack_60 = rtguest::allocFrame(64);
  
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c)));
  if ((uVar1 & 2) != 0) {
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))));
    uVar2 = dpadd(uVar2,0x3ff0000000000000);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))) = uVar3;
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))));
    uVar2 = dpadd(uVar2,0x3ff0000000000000);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))) = uVar3;
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x16c))));
    uVar2 = dpdiv(0x4079000000000000,uVar2);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x158))) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x38))) = uVar3;
  }
  if ((uVar1 & 8) == 0) {
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)));
  }
  else {
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))));
    uVar2 = dpsub(uVar2,0x3ff0000000000000);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x3c))) = uVar3;
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))));
    uVar2 = dpsub(uVar2,0x3ff0000000000000);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c))) = uVar3;
    uVar2 = fptodp((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x16c))));
    uVar2 = dpdiv(0x4079000000000000,uVar2);
    uVar3 = dptofp(uVar2);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x158))) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x38))) = uVar3;
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x15c)));
  }
  uVar2 = fptodp(uVar3);
  ::sprintf((char *)rtguest::G((uint32_t)(auStack_60)), (char *)rtguest::G((uint32_t)(0x32f8c0)), uVar2, (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x170))));
  uiDebugText8(0x10,0x80,auStack_60,0xf);
  return 0;
}


RT_GUEST_FN(0x0026ae88, guest::ApplyAdjusterScales)

}  // namespace guest
