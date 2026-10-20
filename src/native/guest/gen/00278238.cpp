// guest:0x00278238 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdSearchFile(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(176);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0xa0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xa4) = 0x00000000u;
  }
  char cVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined4 uVar5;
  int iVar6;
  long lVar7;
  
  RT_CALLRA(0x27826c, guest::cmd_sem_init());
  iVar6 = RT_CALLRA(0x278278, guest::PollSema((*(int32_t*)rtguest::G(0x2e6f28))));
  if ((*(int32_t*)rtguest::G(0x2e6f28)) == iVar6) {
    (*(uint32_t*)rtguest::G(0x2e6f1c)) = 1;
    RT_CALLRA(0x2782a4, guest::ReferThreadStatus((*(uint32_t*)rtguest::G(0x1773590)),0x1773598));
    lVar7 = RT_CALLRA(0x2782ac, guest::sceCdSync(1));
    if (lVar7 == 0) {
      RT_CALLRA(0x2782bc, guest::sceSifInitRpc(0));
      if ((*(int32_t*)rtguest::G(0x2e6f40)) < 0) {
        while( true ) {
          while (lVar7 = sceSifBindRpc(0x1773780,0xffffffff80000597,0), lVar7 < 0) {
            if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
              scePrintf(0x331bc8);
            }
            iVar6 = 0x100000;
            do {
              iVar6 = iVar6 + -1;
            } while (iVar6 != -1);
          }
          iVar6 = 0x100000;
          if ((*(uint32_t*)rtguest::G(0x17737a4)) != 0) break;
          do {
            iVar6 = iVar6 + -1;
          } while (iVar6 != -1);
        }
        (*(uint32_t*)rtguest::G(0x2e6f40)) = 0;
      }
      (*(uint8_t*)rtguest::G(0x1773624)) = (*(int8_t*)rtguest::G((uint32_t)param_2));
      iVar6 = 0;
      if ((*(int8_t*)rtguest::G(0x1773624)) != '\0') {
        for (iVar6 = 1;
            (iVar6 < 0x100 &&
            (cVar1 = (*(int8_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(iVar6) * sizeof(int8_t))), (*(uint8_t*)rtguest::G((uint32_t)0x1773624 + (uint32_t)(iVar6) * sizeof(uint8_t))) = cVar1, cVar1 != '\0'));
            iVar6 = iVar6 + 1) {
        }
      }
      if (iVar6 == 0x100) {
        (*(uint8_t*)rtguest::G(0x1773723)) = 0;
      }
      (*(uint32_t*)rtguest::G(0x1773724)) = 0x01773600;
      if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
        scePrintf(0x331be8,0x1773624);
      }
      sceSifWriteBackDCache(0x1773600,0x128);
      lVar7 = RT_CALLRA(0x278428, guest::sceSifCallRpc(0x1773780,0,0,0x1773600,0x128,0x1773740,4,0));
      uVar4 = (*(uint64_t*)rtguest::G(0x21773618));
      uVar3 = (*(uint64_t*)rtguest::G(0x21773610));
      uVar2 = (*(uint64_t*)rtguest::G(0x21773608));
      if (-1 < lVar7) {
        (*(uint64_t*)rtguest::G((uint32_t)param_1)) = (*(uint64_t*)rtguest::G(0x21773600));
        (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint64_t))) = uVar2;
        (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint64_t))) = uVar3;
        (*(uint64_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(uint64_t))) = uVar4;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + (uint32_t)(4) * sizeof(uint64_t)))) = (*(uint32_t*)rtguest::G(0x21773620));
        if (((0 < (*(uint32_t*)rtguest::G(0x002e6f10))) && (scePrintf(0x331c00,param_1 + (uint32_t)(1) * sizeof(uint64_t)), 0 < (*(uint32_t*)rtguest::G(0x002e6f10)))) &&
           (scePrintf(0x331c10,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 4)))), 0 < (*(uint32_t*)rtguest::G(0x002e6f10)))) {
          scePrintf(0x331c20,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_1)));
        }
        uVar5 = (*(uint32_t*)rtguest::G(0x21773740));
        RT_CALLRA(0x27843c, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f28))));
        return uVar5;
      }
    }
    RT_CALLRA(0x2784f8, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f28))));
  }
                    /* WARNING: Read-only address (ram,0x21773600) is written */
                    /* WARNING: Read-only address (ram,0x21773608) is written */
                    /* WARNING: Read-only address (ram,0x21773610) is written */
                    /* WARNING: Read-only address (ram,0x21773618) is written */
                    /* WARNING: Read-only address (ram,0x21773620) is written */
                    /* WARNING: Read-only address (ram,0x21773740) is written */
  return 0;
}


RT_GUEST_FN(0x00278238, guest::sceCdSearchFile)

}  // namespace guest
