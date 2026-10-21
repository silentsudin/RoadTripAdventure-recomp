// guest:0x0026dfb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// normalized branch: the two extra fields are guest div.s results fed as float bits.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void PrintVec3Norm(uint32_t param_1, int param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x78) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x7c) = 0x00000000u;
  }
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  float fVar6;
  uint32_t auStack_90 = rtguest::allocFrame(64);

  fVar6 = (*(float*)rtguest::G((uint32_t)(param_1 + 8)));
  if (fVar6 == 0.0f) {
    uVar3 = RT_CALLRA(0x26dffc, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1)))));
    uVar4 = RT_CALLRA(0x26e008, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 4)))));
    uVar5 = RT_CALLRA(0x26e014, guest::fptodp(rtguest::f2bits(fVar6)));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_90)), (char *)rtguest::G((uint32_t)(0x32fc08)),
              uVar3, uVar4, uVar5);
  }
  else {
    uVar3 = RT_CALLRA(0x26e030, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1)))));
    uVar4 = RT_CALLRA(0x26e04c, guest::fptodp((*(uint32_t*)rtguest::G((uint32_t)(param_1 + 4)))));
    uVar5 = RT_CALLRA(0x26e088, guest::fptodp(rtguest::f2bits(fVar6)));
    uVar1 = RT_CALLRA(0x26e094, guest::fptodp(rtguest::f2bits(rtguest::fdiv((*(float*)rtguest::G((uint32_t)(param_1))), fVar6))));
    uVar2 = RT_CALLRA(0x26e0a0, guest::fptodp(rtguest::f2bits(rtguest::fdiv((*(float*)rtguest::G((uint32_t)(param_1 + 4))), fVar6))));
    ::sprintf((char *)rtguest::G((uint32_t)(auStack_90)), (char *)rtguest::G((uint32_t)(0x32fbe0)),
              uVar3, uVar4, uVar5, uVar1, uVar2);
  }
  RT_CALLRA(0x26e0cc, guest::uiDebugText8(0x10, param_2 << 3, auStack_90, 0xf));
  return;
}


RT_GUEST_FN(0x0026dfb0, guest::PrintVec3Norm)

}  // namespace guest
