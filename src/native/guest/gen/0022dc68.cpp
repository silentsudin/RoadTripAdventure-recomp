// guest:0x0022dc68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FinishOnCarApproach(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  ushort uVar1;
  uint uVar2;
  int iVar3;
uint32_t puVar4;
  int iVar5;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 4) != 0) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
    puVar4 = 0x177ac50;
    iVar5 = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 1))) != '\0') {
      do {
        uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x33)));
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x33))) == 0) {
LAB_0022dd94:
          uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 1)));
        }
        else if ((uVar1 & 0x200) == 0) {
          if ((*(float*)rtguest::G((uint32_t)(uint32_t)((int)puVar4 + 0x94))) < 110.0) {
            iVar3 = (int)param_1;
            if ((uVar1 & 0x73) != 0) {
              (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18))) + 0x25))) =
                   (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18))) + 0x25))) |
                   (uint32_t)(1 << ((uVar1 & 0x30) >> 4) + (uint32_t)((uVar1 & 0x40) != 0) * 3);
            }
            if (iVar5 == 0) {
              PlayBgmSlot5();
              SoundStopAllChannels();
              (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
              uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) = 0;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x22eac8;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar2 & 0xfffffeff | 0x10000;
              gs_tasks_kill(param_1);
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
            }
            else {
              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar4 + 0x33))) = 0;
              (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar4 + 0x247))) = 0xff;
              iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
            }
            goto LAB_0022dd94;
          }
          uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 1)));
        }
        else {
          uVar2 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 1)));
        }
        iVar5 = iVar5 + 1;
        puVar4 = puVar4 + 0x270;  // T* element step
      } while (iVar5 < (int)uVar2);
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022dc68, guest::FinishOnCarApproach)

}  // namespace guest
