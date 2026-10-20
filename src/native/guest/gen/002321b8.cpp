// guest:0x002321b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raceHudInit(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(param_1)), 0, 0x8cb0));
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8004))) = (int)param_2;
  if (param_2 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84b4))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84bc))) = 0x34;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84c4))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84a0))) = 0x1ea00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84a8))) = 0x29f66800000014;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84b0))) = 2;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84cc))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84d4))) = 0x4c;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84dc))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84e4))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84ec))) = 100;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84f4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8500))) = 0x28;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8504))) = 0x58;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x850c))) = param_3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84fc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8508))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8510))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8518))) = 0x58;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8520))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8524))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x852c))) = 0x68;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8534))) = 0;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84b4))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84bc))) = 0x34;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84c4))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84a0))) = 0x1ea00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84a8))) = 0x29f66800000014;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84b0))) = 2;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84cc))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84d4))) = 0x4c;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84dc))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84e4))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84ec))) = 100;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84f4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8504))) = 0x58;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x84fc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8500))) = 0x28;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8508))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x850c))) = param_3;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88bc))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88c4))) = 0x10a;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88cc))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8510))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8518))) = 0x58;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8520))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88d4))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88dc))) = 0x122;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88e4))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8524))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x852c))) = 0x68;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8534))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88a8))) = 0x1ea00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88b0))) = 0x29f668000000ea;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 35000))) = 2;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88ec))) = 0x1ee00000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88f4))) = 0x13a;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x88fc))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x890c))) = 400;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8908))) = 0x28;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8914))) = param_3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8904))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8910))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8918))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8920))) = 400;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8928))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x892c))) = 0x8000000000;
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x8934))) = 0x1a0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x893c))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x002321b8, guest::raceHudInit)

}  // namespace guest
