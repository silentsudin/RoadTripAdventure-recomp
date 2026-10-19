// guest:0x00225120 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarEffects(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  int iVar1;
  uint uVar2;
uint32_t puVar3;
uint32_t puVar4;
  int iVar5;
  int iVar6;
  
  iVar5 = (int)param_1 + 0x170;
  iVar6 = 0;
  vifUploadCamera();
  vifUploadAt16(0x177f5e0);
  vifDirect(0x29d3b0,4);
  puVar3 = 0x177ac50;
  do {
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x33))) != 0) {
      if (iVar6 < 3) {
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x20e))) != 0) {
          drawCarEffectA(puVar3,(*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x20e))),(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x194)))
                         ,param_3);
        }
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19c)));
      }
      else {
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x19c)));
      }
      uVar2 = iVar1 >> 8 & 7;
      if (uVar2 == 1) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) != 0) {
          SpawnCarSmoke(puVar3,param_2,iVar5,puVar3 + 0x41,(int)puVar3 + 0x20c,param_1);
        }
      }
      else if (uVar2 < 2) {
        if (uVar2 == 0) {
          if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) != 0) {
            SpawnCarDustPuffs(puVar3,param_2,iVar5,puVar3 + 0x41,(int)puVar3 + 0x20c);
          }
        }
        else {
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) = 0;
        }
      }
      else if (uVar2 == 2) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) != 0) {
          SpawnCarSparks(puVar3,param_2,iVar5,puVar3 + 0x41,(int)puVar3 + 0x20c,param_1);
        }
      }
      else if (uVar2 == 6) {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) != 0) {
          if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x3f))) & 0x40) == 0) {
            AdvanceCarEmitterStep
                      ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar3 + 0x11c))),puVar3,param_2,iVar5,puVar3 + 0x41,
                       (int)puVar3 + 0x20c,param_1);
          }
          else {
            puVar4 = (uint32_t)param_3;
            if (((*(uint64_t*)rtguest::G((uint32_t)puVar4 + (uint32_t)(5) * sizeof(uint64_t))) & 0x8000) == 0) {
              if (((*(uint64_t*)rtguest::G((uint32_t)puVar4)) & 3) == 0) {
                SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)));
                SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 2);
                SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 4);
                SpawnCarDebrisCycle(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)) + 6);
              }
              SpawnCarEmitterEntry(puVar3,param_1,(*(uint64_t*)rtguest::G((uint32_t)puVar4)),iVar6);
            }
          }
        }
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar3 + 0x41))) = 0;
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
