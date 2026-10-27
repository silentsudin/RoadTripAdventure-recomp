// guest:0x0021ca90 transliterated from Ghidra pseudocode + ELF disassembly.
// auVar2 = _sqc2(in_vf0) is the EE sqc2 at 0x21cb08 (misassembled bbit132):
// a 16-byte store of VU0 VF0. The reset block is inline here (0x21cb60),
// not a tail call to ResetVehicleState 0x21c998 (jr $ra at 0x21cbb0).
#include "../guest_vu0.h"
#include "guest_decls.inc"

namespace guest {

void UpdateObjectPhysicsState(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  using namespace rtguest::vu0;
  ushort uVar1;
  int iVar3;
  float fVar4;
  float fVar5;

  RT_CALLRA(0x21caa4, guest::SpawnCarAtDoor(param_1, param_2));  // W13c19: fn/0021ca90.c:16 shows SpawnCarAtDoor() (Ghidra dropped a0/a1; callee fn/002182b0.c takes (param_1,param_2)); MIPS 0x21ca98 move $17,$5 / 0x21caa8 move $16,$4 delay slot: a0,a1 untouched at jal 0x2182b0 @0x21caa4
  iVar3 = (int)param_2;
  fVar4 = (*(float*)rtguest::G((uint32_t)(iVar3 + 0x90))) * (*(float*)rtguest::G(0x00332c28));
  fVar5 = (*(float*)rtguest::G((uint32_t)(iVar3 + 0x98))) * (*(float*)rtguest::G(0x00332c28));
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa4))) =
      (int32_t)((*(float*)rtguest::G((uint32_t)(iVar3 + 0x94))) * (*(float*)rtguest::G(0x00332c28)));
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa0))) =
      (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa0))) + (int32_t)fVar4;
  (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) =
      (*(int32_t*)rtguest::G((uint32_t)(iVar3 + 0xa8))) + (int32_t)fVar5;
  RT_CALLRA(0x21cafc, guest::LoadCarPerformanceSpecs(param_2,param_1));
  sqc2((uint32_t)(iVar3 + 0xf0),S.vf[0]);
  (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1ff))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c4))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1c0))) = 0;
  (*(uint8_t*)rtguest::G((uint32_t)(iVar3 + 0x1fe))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d0))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1ce))) = 0;
  (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x1d2))) = 0;
  if ((*(int16_t*)rtguest::G((uint32_t)(iVar3 + 0x20e))) != 0) {
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)));
    if ((*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)) & 3) == 0) goto LAB_0021cb60;
    RT_CALLRA(0x21cb54, guest::PlayUiSound((int)((short)uVar1 & 1U) + 0x8010));
  }
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x198)));
LAB_0021cb60:
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x100))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x208))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1f0))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1ec))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x10c))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x108))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x104))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1dc))) = 0x1000;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e4))) = 0x1000;
  (*(uint32_t*)rtguest::G((uint32_t)(iVar3 + 0x1e0))) = 0x1000;
  (*(uint16_t*)rtguest::G((uint32_t)(iVar3 + 0x20e))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(0x00335728 + (uint32_t)(uVar1 & 1) * 4))) = 0;
  return;
}


RT_GUEST_FN(0x0021ca90, guest::UpdateObjectPhysicsState)

}  // namespace guest
