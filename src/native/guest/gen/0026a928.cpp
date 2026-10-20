// guest:0x0026a928 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t endingCreditsScroll(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  undefined2 uVar1;
  undefined4 uVar2;
  char cVar3;
  int iVar4;
  int iVar5;
uint32_t pcVar6;
uint32_t puVar7;
uint32_t puVar8;
uint32_t pfVar9;
  float fVar10;
  
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  puVar8 = (uint32_t)0x2dc938;
  if (iVar5 == 0) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x01772488)), 0, 0x1e0));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = 0;
  do {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) + (int)(short)(*(uint16_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint16_t)));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) = iVar4;
    if (iVar5 == iVar4) {
      iVar4 = 0;
      puVar7 = 0x01772488;
      iVar5 = 0;
      pcVar6 = 0x01772490;
      do {
        cVar3 = (*(int8_t*)rtguest::G((uint32_t)pcVar6));
        pcVar6 = pcVar6 + 0x14;
        if (cVar3 == '\0') {
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(puVar7 + 4))) = 1;
          uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 2)));
          (*(uint16_t*)rtguest::G((uint32_t)puVar7)) = (*(uint16_t*)rtguest::G((uint32_t)puVar8));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177248c + iVar5))) = 0x43e00000;
          uVar1 = (*(uint16_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(4) * sizeof(uint16_t)));
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01772494 + iVar5))) = uVar2;
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01772498 + iVar5))) = uVar1;
          break;
        }
        iVar4 = iVar4 + 1;
        puVar7 = puVar7 + 10;
        iVar5 = iVar5 + 0x14;
      } while (iVar4 < 0x18);
      if (iVar4 == 0x18) {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x32f890u)));
        goto LAB_0026a9e4;
      }
      pcVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 2)));
    }
    else {
LAB_0026a9e4:
      pcVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 2)));
    }
    puVar8 = puVar8 + 6;
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar6)) == ' ') break;
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  } while( true );
  iVar5 = 0;
  do {
    while( true ) {
      if (0x17 < iVar5) {
        return 0;
      }
      iVar4 = iVar5 * 0x14;
      if ((*(uint32_t*)rtguest::G((uint32_t)0x1772490 + (uint32_t)(iVar4) * 4)) != '\0') break;
LAB_0026aba4:
      iVar5 = iVar5 + 1;
    }
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x01772498 + iVar4))) == -1) {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
      if (200.0 <= (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177248c + iVar4)))) goto LAB_0026aae0;
      if (cVar3 == '\0') {
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x335468u)), (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) / 0x3c);
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x32f8b0u)), (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26))) = 1;
    }
    else {
      cVar3 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x26)));
LAB_0026aae0:
      if (cVar3 == '\0') {
        (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177248c + iVar5 * 0x14))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0177248c + iVar5 * 0x14))) - 0.5;
      }
    }
    iVar4 = iVar5 * 0x14;
    pfVar9 = (uint32_t)(0x0177248c + iVar4);
    fVar10 = (*(float*)rtguest::G((uint32_t)pfVar9));
    if (fVar10 < -32.0) {
      (*(uint32_t*)rtguest::G((uint32_t)0x1772490 + (uint32_t)(iVar4) * 4)) = 0;
      fVar10 = (*(float*)rtguest::G((uint32_t)pfVar9));
    }
    RT_CALLRA(0x26ab6c, guest::uiDrawText((*(uint32_t*)rtguest::G((uint32_t)0x1772488 + (uint32_t)(iVar5 * 10) * 4)),(int)fVar10 / 2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x01772494 + iVar4))),
               (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01772498 + iVar4)))));
    if ((*(float*)rtguest::G((uint32_t)pfVar9)) + (*(float*)rtguest::G((uint32_t)pfVar9)) == 0.0) {
      (*(float*)rtguest::G((uint32_t)pfVar9)) = (float)0xbfc00000;
      goto LAB_0026aba4;
    }
    iVar5 = iVar5 + 1;
  } while( true );
}


RT_GUEST_FN(0x0026a928, guest::endingCreditsScroll)

}  // namespace guest
