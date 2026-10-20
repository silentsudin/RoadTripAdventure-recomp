// guest:0x002179d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateCarCarCollisionsTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(112);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x48) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4c) = 0x00000000u;
  }
    uint32_t fStack_70 = rtguest::allocFrame(4);
    uint32_t fStack_6c = rtguest::allocFrame(4);
    uint32_t fStack_68 = rtguest::allocFrame(4);
    uint32_t fStack_64 = rtguest::allocFrame(4);
  int iVar1;
uint32_t puVar2;
  long lVar3;
  int iVar4;
uint32_t puVar5;
uint32_t puVar6;
  int iVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x28))) & 0x48000) == 0) {
    iVar7 = 0;
    puVar6 = 0x177ac50;
    do {
      iVar7 = iVar7 + 1;
      iVar1 = iVar7;
      puVar2 = puVar6;
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) & 0x7fff) != 0) {
        for (; iVar1 < 0x18; iVar1 = iVar1 + 1) {
          puVar5 = puVar2 + 0x4e * sizeof(uint64_t);  // fn:34 undefined8* elem step (cf. :73 0x270)
          if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x81))) & 0x7fff) != 0) {
            fVar9 = 0.0;
            RT_CALLRA(0x217a78, guest::VecSubtractAndLength(fStack_70,puVar2 + 0x62,puVar6 + 0x14));
            if ((((*(float*)rtguest::G((uint32_t)fStack_64)) != fVar9) && ((*(float*)rtguest::G((uint32_t)fStack_64)) < (*(float*)rtguest::G(0x332b6c)))) &&
               ((lVar3 = TestCarCollisionBox(puVar5,puVar6), lVar3 != 0 ||
                (lVar3 = TestCarCollisionBox(puVar6,puVar5), lVar3 != 0)))) {
              fVar10 = ((*(uint32_t*)rtguest::G(0x332b70)) - (*(float*)rtguest::G((uint32_t)fStack_64))) / (*(float*)rtguest::G((uint32_t)fStack_64));
              fVar11 = (*(uint32_t*)rtguest::G(0x332b74));
              if (fVar9 <= fVar10) {
                fVar11 = fVar10;
              }
              if ((((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x33))) & 3) != 0) || (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x81))) & 3) != 0)
                 ) {
                RT_CALLRA(0x217b20, guest::HandleCarCollisionResponse(param_1,param_2,puVar6,puVar5,iVar1));
              }
              fVar10 = (*(float*)rtguest::G((uint32_t)fStack_70)) * fVar11 * 32768.0;
              fVar8 = (*(float*)rtguest::G((uint32_t)fStack_6c)) * fVar11 * 32768.0;
              fVar9 = (*(float*)rtguest::G((uint32_t)fStack_68)) * fVar11 * 32768.0;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x14))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x14))) - fVar10);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xa4))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar6 + 0xa4))) - fVar8);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x15))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x15))) - fVar9);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x62))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x62))) + fVar10);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 0x314))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 0x314))) + fVar8);
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 99))) = (int)((float)(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 99))) + fVar9);
              iVar4 = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1e))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x6c)))) / 2;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1e))) = iVar4;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1f))) = ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1f))) + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x6d)))) / 2;
              (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x6c))) = iVar4;
              (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar2 + 0x6d))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar6 + 0x1f)));
            }
          }
          puVar2 = puVar5;
        }
      }
      puVar6 = puVar6 + 0x270;  // T* element step
    } while (iVar7 < 0x17);
  }
  return 0;
}


RT_GUEST_FN(0x002179d8, guest::UpdateCarCarCollisionsTask)

}  // namespace guest
