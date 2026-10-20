// guest:0x0022ad70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ClassifyObjectContext(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  char cVar1;
  char cVar2;
  undefined4 uVar3;
  ulong uVar4;
  
  cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x21)));
  if ((cVar2 == '\x01') &&
     ((((cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23))), cVar1 < '\x04' || (cVar1 == '\x05')) ||
       (cVar1 == '\x06')) || (((cVar1 == '\a' || (cVar1 == '\b')) || (cVar1 == '\t')))))) {
    return 1;
  }
  if ((cVar2 == '\x02') &&
     (((cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23))), cVar1 < '\x04' || (cVar1 == '\x06')) ||
      ((cVar1 == '\a' || ((cVar1 == '\b' || (cVar1 == '\t')))))))) {
    return 2;
  }
  if (cVar2 == '\x03') {
    cVar2 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x23)));
    if ((((cVar2 < '\x04') || (cVar2 == '\x06')) || (cVar2 == '\b')) || (cVar2 == '\t')) {
      return 3;
    }
    uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  }
  else {
    uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  }
  uVar3 = 4;
  if (((uVar4 & 0xff00ff00) != 0x9000500) && (uVar3 = 5, (uVar4 & 0xff00ff00) != 0x9000700)) {
    uVar3 = 0;
  }
  return uVar3;
}


RT_GUEST_FN(0x0022ad70, guest::ClassifyObjectContext)

}  // namespace guest
