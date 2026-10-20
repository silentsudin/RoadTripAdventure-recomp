// guest:0x00243c50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildCarouselSlotPackets(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
uint32_t puVar2;
  long lVar3;
  ulong uVar4;
  ulong uVar5;
  int iVar6;
uint32_t puVar7;
  int iVar8;
uint32_t puVar9;
  int iVar10;
uint32_t psVar11;
  int iVar12;
  long lVar13;
  
  if (param_2 == 0) {
    psVar11 = (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))) * 8 + 0x2a8c00);
  }
  else {
    psVar11 = (uint32_t)(0x002a8c78 + (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))) * 8);
  }
  iVar10 = param_1 + 0x2b80;
  lVar13 = 0;
  puVar2 = 0x002f03f0;
  do {
    if (0xf < lVar13) {
      return 0;
    }
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400)));
    iVar12 = (int)lVar13;
    if (iVar6 == 0) {
      if (lVar13 == 8) {
        gifSetTexRegs(iVar10,(long)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(3) * sizeof(int16_t))) << 0x14 | (long)((*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(1) * sizeof(int16_t))) + 0x243e) << 0x25 |
                             0x20000005dc00a43e,5);
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) & 0xfffffffffffffff0 | 7;
        return 0;
      }
      uVar4 = (long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) | 0x2000000000008000U |
              (long)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(3) * sizeof(int16_t))) << 0x14 | 0x5dc000000U |
              (long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + (int)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(1) * sizeof(int16_t))) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) << 0x25;
LAB_00243d68:
      gifSetTexRegs(iVar10,uVar4,5);
      (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) & 0xfffffffffffffff0 | 7;
    }
    else {
      if ((iVar6 - 0xbU < 2) || (iVar6 == 8)) {
        lVar3 = GetPartValueU16(iVar6,lVar13);
        if (lVar3 != 9) {
          lVar13 = GetPartValueU16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x5400))),lVar13);
          gifSetTexRegs(iVar10,0x20137c45dd309bc2,lVar13 << 0x38 | 0x137c45dd409bc2);
          uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) & 0xfffffffffffffff0 | 7;
          iVar6 = 0x70;
          goto LAB_00243f04;
        }
        uVar4 = MakeVifCall15xGif((*(uint32_t*)rtguest::G((uint32_t)puVar2)));
        goto LAB_00243d68;
      }
      if (iVar6 == 2) {
        gifSetTexRegs(iVar10,(long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) | 0x2000000000008000U |
                             (long)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(3) * sizeof(int16_t))) << 0x14 | 0x59c000000U |
                             (long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + (int)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(1) * sizeof(int16_t))) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) << 0x25,5
                     );
      }
      else {
        if (iVar6 - 3U < 2) {
          sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)));
        }
        else {
          if (iVar6 != 6) {
            gifSetTexRegs(iVar10,(long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) | 0x2000000000004000U |
                                 (long)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(3) * sizeof(int16_t))) << 0x14 | 0x598000000U |
                                 (long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + (int)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(1) * sizeof(int16_t))) + iVar12 * (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)))) <<
                                 0x25,5);
            goto LAB_00243f70;
          }
          sVar1 = (*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(2) * sizeof(int16_t)));
        }
        uVar4 = (uint32_t)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + iVar12 * sVar1);
        uVar5 = (long)((int)(*(int16_t*)rtguest::G((uint32_t)psVar11)) + (int)(*(int16_t*)rtguest::G((uint32_t)psVar11 + (uint32_t)(1) * sizeof(int16_t))) + iVar12 * sVar1) << 0x25;
        gifSetTexRegs(iVar10,uVar4 | 0x19304000 | uVar5 | 0x2010000580000000,
                      uVar4 | 0x19404000 | lVar13 << 0x38 | uVar5 | 0x10000580000000);
        uVar4 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8)));
        iVar6 = 0x60;
LAB_00243f04:
        (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 8))) = uVar4 & 0xffffffffffffff0f | (long)iVar6;
      }
    }
LAB_00243f70:
    gifTexQuadTag(iVar10 + 0x20);
    puVar7 = (uint32_t)(iVar10 + 0x5c);
    iVar8 = 3;
    puVar9 = 0x002a8ae0;
    iVar6 = iVar10;
    do {
      iVar6 = iVar6 + 0x30;
      iVar8 = iVar8 + -1;
      vu0CopyVector(iVar6,puVar9);
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 0xff0;
      puVar7 = puVar7 + 0x30;  // T* element step
      puVar9 = puVar9 + 0x10;
    } while (-1 < iVar8);
    iVar10 = iVar10 + 0xf0;
    lVar13 = (long)(iVar12 + 1);
    puVar2 = puVar2 + 0x14;  // T* element step
  } while( true );
}


RT_GUEST_FN(0x00243c50, guest::BuildCarouselSlotPackets)

}  // namespace guest
