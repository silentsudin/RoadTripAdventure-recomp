// guest:0x00286130 transliterated from Ghidra pseudocode — DO NOT hand-edit
// sceTtyHandler: deci2 tty ring pump (recv into buffer, push bytes into guest ring).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceTtyHandler(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
  }
  uint32_t puVar1;
  uint32_t puVar2;
  int32_t lVar3;
  uint32_t puVar4;
  int32_t iVar5;
  int32_t iVar6;

  if (param_1 == 3) {
    lVar3 = (int32_t)RT_CALLRA(0x286248, guest::sceDeci2ExSend((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(0) * 4)),
                                    (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * 4)),
                                    ((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * 4)) & 0xffff)));
    if (-1 < lVar3) {
      (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * 4)) =
           (int32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * 4)) + lVar3;
      (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * 4)) =
           (int32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * 4)) - lVar3;
      return 0;
    }
    printf(0x3325b0);
  }
  else {
    if (param_1 < 4) {
      if (param_1 < 1) {
        return 0;
      }
      if (param_2 == 0) {
        puVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(5) * 4));
        iVar6 = 0xc;
        if (0xc < (int32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar1))) {
          iVar5 = (int32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(6) * 4));
          puVar4 = puVar1;
          while (true) {
            puVar2 = (uint32_t)puVar4 + (uint32_t)iVar6;
            iVar6 = iVar6 + 1;
            (*(uint8_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)((int32_t)iVar5 + 0xc))))) =
                 (*(uint8_t*)rtguest::G(puVar2));
            RT_CALLRA(0x286218, guest::ttyRingPush((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(6) * 4))));
            if ((int32_t)(uint32_t)(*(uint16_t*)rtguest::G((uint32_t)puVar1)) <= iVar6) break;
            puVar4 = (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(5) * 4));
            iVar5 = (int32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(6) * 4));
          }
        }
        (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * 4)) = 0;
        return 0;
      }
      if (0x140 < (uint32_t)((*(int32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * 4)) +
                             (int32_t)param_2)) {
        printf(0x332570);
      }
      lVar3 = (int32_t)RT_CALLRA(0x2861ac, guest::sceDeci2ExRecv(
           (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(0) * 4)),
           ((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(5) * 4)) +
            (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * 4))),
           (param_2 & 0xffff)));
      if (lVar3 < 0) {
        printf(0x332598);
      }
      (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * 4)) =
           (int32_t)(*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * 4)) + lVar3;
      return 0;
    }
    if (param_1 != 4) {
      return 0;
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * 4)) != 0) {
      printf(0x3325c8, (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * 4)));
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * 4)) = 0;
  return 0;
}


RT_GUEST_FN(0x00286130, guest::sceTtyHandler)

}  // namespace guest
