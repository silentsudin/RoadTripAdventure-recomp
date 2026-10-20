// guest:0x00220030 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawFieldTilesExtra(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  int iVar1;
  int iVar2;
  int iVar3;
  
  if ((-1 < (*(int8_t*)rtguest::G(0x3335b8))) && (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x60))))) {
    RT_CALLRA(0x220064, guest::vifSetBatchOffset(0x29d080 + (*(int8_t*)rtguest::G(0x3335b8)) * 0x10));
    RT_CALLRA(0x220074, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ab20)) + 0x100)))));
  }
  if ((-1 < (*(uint32_t*)rtguest::G(0x3335b9))) && (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 100))))) {
    RT_CALLRA(0x2200a0, guest::vifSetBatchOffset(0x29d080 + (*(uint32_t*)rtguest::G(0x3335b9)) * 0x10));
    RT_CALLRA(0x2200b0, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab24)) + 0x100)))));
  }
  if ((-1 < (*(uint32_t*)rtguest::G(0x3335ba))) && (-1 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x68))))) {
    RT_CALLRA(0x2200dc, guest::vifSetBatchOffset(0x29d080 + (*(uint32_t*)rtguest::G(0x3335ba)) * 0x10));
    RT_CALLRA(0x2200ec, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ab28)) + 0x100)))));
  }
  if ((*(uint32_t*)rtguest::G(0x3335bb)) < '\0') {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x6c)));
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x6c))) < 0) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x6c)));
  }
  else {
    RT_CALLRA(0x220120, guest::vifSetBatchOffset(0x29d080 + (*(uint32_t*)rtguest::G(0x3335bb)) * 0x10));
    RT_CALLRA(0x220130, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ab2c)) + 0x100)))));
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x6c)));
  }
  if ((iVar3 == 0xb) && (-1 < (*(uint32_t*)rtguest::G(0x3335bb)))) {
    iVar3 = 0;
    RT_CALLRA(0x220164, guest::vifSetBatchOffset(0x29d080 + (*(uint32_t*)rtguest::G(0x3335bb)) * 0x10));
    do {
      iVar1 = RT_CALLRA(0x220178, guest::TestProgressFlag(iVar3));
      iVar2 = iVar3 * 2;
      iVar3 = iVar3 + 1;
      RT_CALLRA(0x220198, guest::vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ab80 + (uint32_t)(iVar2 + iVar1) * 4)) + 0x100)))));
    } while (iVar3 < 0x1a);
  }
  return 0;
}


RT_GUEST_FN(0x00220030, guest::drawFieldTilesExtra)

}  // namespace guest
