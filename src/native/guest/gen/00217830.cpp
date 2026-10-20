// guest:0x00217830 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HandleCarCollisionResponse(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
uint32_t auStack_40 = rtguest::allocFrame(12);
    uint32_t fStack_34 = rtguest::allocFrame(4);
  byte bVar1;
  ushort uVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  
  iVar5 = (int)param_4;
  iVar7 = (int)param_3;
  RT_CALLRA(0x217868, guest::VecSubtractAndLength(auStack_40,iVar5 + 0xf0,iVar7 + 0xf0));
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x198)));
  (*(float*)rtguest::G((uint32_t)fStack_34)) = (*(float*)rtguest::G((uint32_t)fStack_34)) * 1000.0;
  iVar6 = (int)param_2;
  if ((uVar2 & 3) != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) == 0) {
      uVar4 = 8;
      if (((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0x40) == 0) && (uVar4 = 8, (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x210))) == 0))
         && (uVar4 = 8, (uVar2 & 0xfb0) == 0)) {
        RT_CALLRA(0x21793c, guest::gs_controls_rumble_start(param_2,uVar2 & 1,6,(int)(*(float*)rtguest::G((uint32_t)fStack_34))));
        RT_CALLRA(0x217944, guest::PlayUiSound(0x14));
        uVar4 = 8;
      }
    }
    else {
      uVar4 = 0xb4;
      if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x210))) == 0) {
        bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x247)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x23))) = bVar1;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x21))) = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)((int)((uint32_t)bVar1 << 0x18) >> 0x15) * 1)) + param_5 + -1
        ;
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 2;
        RT_CALLRA(0x2178e8, guest::ResetVehicleState(param_2,param_3));
        RT_CALLRA(0x2178f4, guest::ResetVehicleState(param_2,param_4));
        uVar4 = 0xb4;
      }
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x210))) = uVar4;
  }
  uVar2 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x198)));
  if ((uVar2 & 3) != 0) {
    if (((((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0x40) == 0) && ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x210))) == 0)) &&
       ((uVar2 & 0xfb0) == 0)) {
      RT_CALLRA(0x2179a0, guest::gs_controls_rumble_start(param_2,uVar2 & 1,6,(int)(*(float*)rtguest::G((uint32_t)fStack_34))));
      RT_CALLRA(0x2179a8, guest::PlayUiSound(0x14));
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x210))) = 8;
  }
  return 0;
}


RT_GUEST_FN(0x00217830, guest::HandleCarCollisionResponse)

}  // namespace guest
