// guest:0x00210158 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnTownEntities(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg;
  char cVar1;
  char cVar2;
  int iVar3;
uint32_t puVar4;
  long lVar5;
uint32_t piVar6;
uint32_t puVar7;
  long lVar8;
  ulong uVar9;
  
  if ((param_1 == 8) || (param_1 == 0x16)) {
    SelectMeshIntersectionRoutine(1);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x22))) = 0x22;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x5c))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x58))) = 0;
  }
  else {
    SelectMeshIntersectionRoutine(0);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x22))) = 0xff;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x21))) = param_2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x6c))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x68))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 100))) = 0xffffffff;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x60))) = 0xffffffff;
  (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x23))) = (char)param_1;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x22))) != -1) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x54))) = 0x40;
  }
  cVar2 = (*(uint32_t*)rtguest::G(0x2be486));
  cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x23)));
  iVar3 = cVar1 * 8;
  if (cVar1 == '\0') {
    lVar8 = 0;
    uVar9 = (uint32_t)(*(int8_t*)rtguest::G(0x2be487));
    if (0 < (long)uVar9) {
      puVar7 = 0x0177ea60;
      do {
        iVar3 = (int)cVar2 + (int)lVar8;
        lVar8 = (long)((int)lVar8 + 1);
        lVar5 = MatchSlotPairState(9,iVar3);
        if (lVar5 == 0) {
          puVar4 = (*(uint32_t*)rtguest::G(0x002c22ec)) + iVar3 * 0x10;
        }
        else {
          puVar4 = (uint32_t)((int)0x177f760 + (int)lVar5 * 0x1c);
        }
        (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = puVar4;
        puVar7 = puVar7 + 0x4;  // T* element step
      } while (lVar8 < (long)uVar9);
    }
    gs_tasks_spawn(param_3,0x20dc50,uVar9 | 0x370400);
  }
  else {
    uVar9 = (uint32_t)(char)(*(uint32_t*)rtguest::G((uint32_t)0x2be43f + (uint32_t)(iVar3) * 4));
    if (uVar9 != 0) {
      cVar2 = (*(uint8_t*)rtguest::G((uint32_t)0x2be43e + (uint32_t)(iVar3) * 1));
      lVar8 = 0;
      if (0 < (long)uVar9) {
        piVar6 = 0x0177ea60;
        do {
          iVar3 = (int)cVar2 + (int)lVar8;
          lVar8 = (long)((int)lVar8 + 1);
          lVar5 = MatchSlotPairState(cVar1,iVar3);
          if (lVar5 == 0) {
            iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + cVar1 * 4))) + iVar3 * 0x10;
          }
          else {
            iVar3 = (int)0x177f760 + (int)lVar5 * 0x1c;
          }
          (*(int32_t*)rtguest::G((uint32_t)piVar6)) = iVar3;
          piVar6 = piVar6 + 0x4;  // T* element step
        } while (lVar8 < (long)uVar9);
      }
      gs_tasks_spawn(param_3,0x20dc50,uVar9 | 0x370400);
    }
  }
  gs_tasks_spawn(param_3,0x20dc50,0x340008);
  if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x23))) == '\b') || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 0x23))) == '\x16')) {
    gs_tasks_spawn(param_3,0x20dc50,0x350000);
  }
  else {
    gs_tasks_spawn(param_3,0x20dc50,(long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002be43c + param_1 * 8))) | 0x390000);
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_3 + 0x27))) = 1;
  return 0;
}


RT_GUEST_FN(0x00210158, guest::SpawnTownEntities)

}  // namespace guest
