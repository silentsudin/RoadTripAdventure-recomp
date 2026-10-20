// guest:0x00243670 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t DispatchByIndex15(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  undefined8 uVar1;
  
  if (param_1 < 0xf) {
                    /* WARNING: Could not recover jumptable at 0x0024368c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    // MIPS 0x243688: lw $v1,0x1280($v1); jr $v1 — RAM table holds targets directly; Ghidra's code** model double-derefs
    uVar1 = rtCall((*(uint32_t*)rtguest::G((uint32_t)(0x2f1280u + (uint32_t)((int)param_1) * 4))));
    return uVar1;
  }
  return 0;
}


RT_GUEST_FN(0x00243670, guest::DispatchByIndex15)

}  // namespace guest
