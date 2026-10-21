// guest:0x00225120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t drawCarEffects(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  uint uVar2;
uint32_t puVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_1 + 0x170;
  iVar6 = 0;
  RT_CALLRA(0x225150, guest::vifUploadCamera());
  RT_CALLRA(0x22515c, guest::vifUploadAt16(0x177f5e0));
  RT_CALLRA(0x22516c, guest::vifDirect(0x29d3b0,4));
  puVar3 = 0x177ac50;
    // W13 fix: puVar3 is undefined8* in fn/00225120.c:30,38,45,60 (puVar3 + 0x33/0x41/0x3f) => 8-byte stride (+0x198/+0x208/+0x1f8)
  do {
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x33 * 8))) != 0) {
      if (iVar6 < 3) {
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x20e))) != 0) {
          RT_CALLRA(0x2251a4, guest::drawCarEffectA(puVar3,(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x20e))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x194)))
                         ,param_3));
        }
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19c)));
      }
      else {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19c)));
      }
      uVar2 = iVar1 >> 8 & 7;
      if (uVar2 == 1) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) != 0) {
          RT_CALLRA(0x22523c, guest::SpawnCarSmoke(puVar3,param_2,iVar5,puVar3 + 0x41 * 8,(int)puVar3 + 0x20c,param_1));
        }
      }
      else if (uVar2 < 2) {
        if (uVar2 == 0) {
          if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) != 0) {
            RT_CALLRA(0x22520c, guest::SpawnCarDustPuffs(puVar3,param_2,iVar5,puVar3 + 0x41 * 8,(int)puVar3 + 0x20c));
          }
        }
        else {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) = 0;
        }
      }
      else if (uVar2 == 2) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) != 0) {
          RT_CALLRA(0x22526c, guest::SpawnCarSparks(puVar3,param_2,iVar5,puVar3 + 0x41 * 8,(int)puVar3 + 0x20c,param_1));
        }
      }
      else if (uVar2 == 6) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) != 0) {
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x3f * 8))) & 0x40) == 0) {
            RT_CALLRA(0x225334, guest::AdvanceCarEmitterStep((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x11c))),puVar3,param_2,iVar5,puVar3 + 0x41 * 8,
                       (int)puVar3 + 0x20c,param_1));
          }
          else {
            puVar4 = (uint32_t)param_3;
            if (((*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(5) * sizeof(uint64_t))) & 0x8000) == 0) {
              if (((*(uint64_t*)rtguest::G((uint32_t)puVar4)) & 3) == 0) {
                RT_CALLRA(0x2252c0, guest::SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4))));
                RT_CALLRA(0x2252d4, guest::SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 2));
                RT_CALLRA(0x2252e8, guest::SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 4));
                RT_CALLRA(0x2252fc, guest::SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 6));
              }
              RT_CALLRA(0x225310, guest::SpawnCarEmitterEntry(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)),iVar6));
            }
          }
        }
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41 * 8))) = 0;
      }
    }
    iVar6 = iVar6 + 1;
    puVar3 = puVar3 + 0x270;  // T* element step
    param_2 = param_2 + 6;
  } while (iVar6 < 0x18);
  UpdateCarEffectSlots(param_2,param_3,iVar5);
  return 0;
}


RT_GUEST_FN(0x00225120, guest::drawCarEffects)

}  // namespace guest
