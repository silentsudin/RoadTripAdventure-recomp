// guest:0x0027cbb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// frame_init (gcc dwarf frame registry): count_fdes sum, two malloc buffers,
// add_fdes per chain entry, end_fdes_sort.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t frame_init(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int32_t iVar1;
  int32_t lVar2;
  int32_t iVar3;
  uint32_t piVar4;
  int32_t iVar5;
  uint32_t pvStack_40 = rtguest::allocFrame(8);  // word1 = uStack_3c (sd high half)
  uint32_t pvStack_38 = rtguest::allocFrame(8);  // word1 = uStack_34 (sd high half)
  uint32_t iStack_30 = rtguest::allocFrame(4);
  uint32_t aiStack_2c = rtguest::allocFrame(16);

  if ((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0) * 4)) == 0) {
    piVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * 4));
    if (piVar4 == 0) {
      iVar5 = (int32_t)count_fdes((*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)));
    }
    else {
      iVar3 = (int32_t)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(0) * 4));
      iVar5 = 0;
      if (iVar3 != 0) {
        do {
          piVar4 = piVar4 + 4;  // int* element step
          iVar1 = (int32_t)count_fdes((uint32_t)iVar3);
          iVar3 = (int32_t)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(0) * 4));
          iVar5 = iVar5 + iVar1;
        } while (iVar3 != 0);
        (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * 4)) = iVar5;
        goto LAB_0027cc28;
      }
    }
  }
  else {
    iVar5 = (int32_t)(*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * 4));
  }
  (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * 4)) = iVar5;
LAB_0027cc28:
  if (iVar5 == 0) {
    (*(uint64_t*)rtguest::G((uint32_t)pvStack_40)) = 0;
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)pvStack_40)) =
         (uint64_t)malloc((uint32_t)((int64_t)iVar5 << 2));
  }
  (*(uint64_t*)rtguest::G((uint32_t)pvStack_38)) = 0;
  if ((*(uint32_t*)rtguest::G((uint32_t)pvStack_40)) != 0) {
    (*(uint64_t*)rtguest::G((uint32_t)pvStack_38)) =
         (uint64_t)malloc((uint32_t)((int64_t)iVar5 << 2));
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)pvStack_40)) != 0) ||
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0) * 4)) == 0) {
    piVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * 4));
    (*(int32_t*)rtguest::G((uint32_t)iStack_30)) = -1;
    (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4)) = 0;
    if (piVar4 == 0) {
      add_fdes((*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * 4)), pvStack_40,
               iStack_30, aiStack_2c);
    }
    else {
      iVar3 = (int32_t)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(0) * 4));
      while (iVar3 != 0) {
        piVar4 = piVar4 + 4;  // int* element step
        add_fdes((uint32_t)iVar3, pvStack_40, iStack_30, aiStack_2c);
        iVar3 = (int32_t)(*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(0) * 4));
      }
    }
    lVar2 = (int32_t)end_fde_sort(pvStack_40, (uint32_t)iVar5);
    if (lVar2 != 0) {
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * 4)) = (int32_t)lVar2;
    }
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0) * 4)) =
         (*(int32_t*)rtguest::G((uint32_t)iStack_30));
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * 4)) =
         (*(int32_t*)rtguest::G((uint32_t)aiStack_2c + (uint32_t)(0) * 4));
  }
  return 0;
}


RT_GUEST_FN(0x0027cbb0, guest::frame_init)

}  // namespace guest
