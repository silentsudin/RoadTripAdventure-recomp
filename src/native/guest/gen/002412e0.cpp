// guest:0x002412e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateConditionalEffects(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
  undefined4 uVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
uint32_t puVar7;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 1) {
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = 0;
  }
  puVar7 = 0x002a3920;
  uVar5 = 0;
  iVar6 = 0;
  do {
    if (((int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) >> (uVar5 & 0x1f) & 1U) == 0) {
      lVar2 = RT_CALLRA(0x241364, guest::tileSlotLoaded(param_2,(*(uint32_t*)rtguest::G((uint32_t)puVar7))));
      if (lVar2 != 0) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a3960))))));
        RT_CALLRA(0x241398, guest::gs_tasks_spawn(param_1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2a3930))),0));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) | (uint32_t)(1 << (uVar5 & 0x1f));
      }
      if (((int)(uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) >> (uVar5 & 0x1f) & 1U) != 0) {
        uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
        iVar4 = iVar6;
        goto LAB_002413d4;
      }
    }
    else {
      uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar7));
      iVar4 = uVar5 << 2;
LAB_002413d4:
      lVar2 = RT_CALLRA(0x2413d4, guest::tileSlotLoaded(param_2,uVar1));
      if (lVar2 == 0) {
        (*(uint8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2a3940u + iVar4))))) = (*(uint32_t*)rtguest::G((uint32_t)0x2a3950 + (uint32_t)(iVar4) * 4));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x26))) & ~(uint32_t)(1 << (uVar5 & 0x1f));
      }
    }
    uVar5 = uVar5 + 1;
    puVar7 = puVar7 + 0x4;  // T* element step
    iVar6 = iVar6 + 4;
    if (2 < (int)uVar5) {
      iVar6 = 0xd;
      RT_CALLRA(0x241420, guest::AdvanceOscillatingValue(iVar3 + 2));
      puVar7 = 0x002a3800;
      do {
        lVar2 = RT_CALLRA(0x241434, guest::tileSlotLoaded(param_2,(*(uint32_t*)rtguest::G((uint32_t)puVar7))));
        if (lVar2 != 0) {
          // MIPS 0x241448: lw $v0,4($s1); jalr $v0 — table holds the handler directly; Ghidra's code** model double-derefs
          rtCall((*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(1) * sizeof(uint32_t))), param_1, param_2);
        }
        iVar6 = iVar6 + -1;
        puVar7 = puVar7 + 0x8;  // T* element step
      } while (-1 < iVar6);
      return 0;
    }
  } while( true );
}


RT_GUEST_FN(0x002412e0, guest::UpdateConditionalEffects)

}  // namespace guest
