// guest:0x00271310 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textGridPrintf(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, uint64_t param_6, uint64_t param_7, uint64_t param_8, ...) {
  rtguest::FrameGuard rt_fg(1216);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x490) = (uint32_t)(int32_t)param_3;
    *(uint32_t*)rtguest::G(rt_sv+0x494) = (uint32_t)((int64_t)(int32_t)param_3 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x498) = (uint32_t)(int32_t)param_4;
    *(uint32_t*)rtguest::G(rt_sv+0x49c) = (uint32_t)((int64_t)(int32_t)param_4 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x4a0) = (uint32_t)(int32_t)param_5;
    *(uint32_t*)rtguest::G(rt_sv+0x4a4) = (uint32_t)((int64_t)(int32_t)param_5 >> 32);
    *(uint32_t*)rtguest::G(rt_sv+0x4a8) = (uint32_t)(int32_t)param_6;
    *(uint32_t*)rtguest::G(rt_sv+0x4ac) = (uint32_t)((int64_t)(int32_t)param_6 >> 32);
  }
    uint32_t bStack_4c0 = rtguest::allocFrame(1);
uint32_t abStack_4bf = rtguest::allocFrame(1023);
rtguest::allocFrame(144);  // guest frame gap
    uint32_t uStack_30 = rtguest::allocFrame(8);
    uint32_t uStack_28 = rtguest::allocFrame(8);
    uint32_t uStack_20 = rtguest::allocFrame(8);
    uint32_t uStack_18 = rtguest::allocFrame(8);
    uint32_t uStack_10 = rtguest::allocFrame(8);
    uint32_t uStack_8 = rtguest::allocFrame(8);
  byte bVar1;
  ushort uVar2;
  undefined8 uVar3;
  uint uVar4;
uint32_t pbVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = 0;
  iVar6 = 0;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_30)) = param_3;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_28)) = param_4;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_20)) = param_5;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_18)) = param_6;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_10)) = param_7;
  (*(uint64_t*)rtguest::G((uint32_t)uStack_8)) = param_8;
  // MIPS 0x271350: jal 0x281068 (in-image vsprintf), $a0=sp (dst), $a1=fmt,
  // $a2=sp+0x490 = varargs spilled via sd (u64 slots, 0x271318..0x271354).
  // The transliterator's ::vsprintf((va_list)(uintptr_t)block) handed the host
  // libc a guest address as va_list (SIGBUS); routed to the guest vsprintf
  // host mirror, which reads the block through G().
  vsprintf(bStack_4c0,param_2,uStack_30);
  uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)bStack_4c0));
  pbVar5 = abStack_4bf;
  if (uVar4 != 0) {
    if (uVar4 != 0x1b) goto LAB_002713d4;
LAB_00271534:
    if (((*(int8_t*)rtguest::G((uint32_t)abStack_4bf + (uint32_t)(0) * 1)) != 0) && ((*(int8_t*)rtguest::G((uint32_t)pbVar5 + (uint32_t)(1) * sizeof(int8_t))) != 0)) {
      uVar2 = CONCAT11((*(int8_t*)rtguest::G((uint32_t)abStack_4bf + (uint32_t)(0) * 1)),(*(int8_t*)rtguest::G((uint32_t)pbVar5 + (uint32_t)(1) * sizeof(int8_t))));
      if (uVar2 != 0x2442) {
        if (0x2442 < uVar2) {
          if ((uVar2 == 0x2840) || (uVar2 == 0x2842)) {
            iVar6 = 3;
            pbVar5 = pbVar5 + 2;
          }
          goto LAB_002714f4;
        }
        if (uVar2 != 0x2440) goto LAB_002714f4;
      }
      iVar6 = 0;
      pbVar5 = pbVar5 + 2;
LAB_002714f4:
      if (param_1 == 0) {
        iVar7 = iVar7 + 1;
      }
      else {
        uVar3 = charCodeToGlyphIndex(uVar4);
        textGridPutChar(param_1,uVar3,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))));
        iVar7 = iVar7 + 1;
      }
      do {
        uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar5));
        while( true ) {
          while( true ) {
            pbVar5 = pbVar5 + 1;
            if (uVar4 == 0) {
              return iVar7;
            }
            if (uVar4 == 0x1b) {
              (*(int8_t*)rtguest::G((uint32_t)abStack_4bf + (uint32_t)(0) * 1)) = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
              goto LAB_00271534;
            }
LAB_002713d4:
            if (uVar4 != 0x26) {
              if (uVar4 < 0x80) {
                if (iVar6 == 3) {
                  bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
                  pbVar5 = pbVar5 + 1;
                  if (bVar1 == 0) {
                    return iVar7;
                  }
                  uVar4 = uVar4 << 8 | (uint32_t)bVar1;
                }
                goto LAB_002714f4;
              }
              bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
              pbVar5 = pbVar5 + 1;
              if (bVar1 == 0) {
                return iVar7;
              }
              if (iVar6 == 1) {
                uVar4 = charCodeToTileCoord();
                goto LAB_002714f4;
              }
              if (iVar6 != 0) {
                if (iVar6 == 2) {
                  uVar4 = xorHighBitPair();
                }
                goto LAB_002714f4;
              }
              if (0x9f < uVar4) {
                if (0xf < (uVar4 - 0xe0 & 0xffff)) {
                  iVar6 = 2;
                  uVar4 = xorHighBitPair();
                  goto LAB_002714f4;
                }
                if (0xa0 < bVar1) {
                  iVar6 = 0;
                  uVar4 = xorHighBitPair();
                  goto LAB_002714f4;
                }
              }
              iVar6 = 1;
              uVar4 = charCodeToTileCoord();
              goto LAB_002714f4;
            }
            bVar1 = (*(int8_t*)rtguest::G((uint32_t)pbVar5));
            pbVar5 = pbVar5 + 1;
            if (bVar1 == 0) {
              return iVar7;
            }
            if ((uint32_t)(bVar1 - 0x30) < 8) break;
            if (bVar1 == 0x26) goto LAB_002714f4;
            uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar5));
          }
          if (param_1 != 0) break;
          uVar4 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)pbVar5));
        }
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = bVar1 - 0x30;
      } while( true );
    }
  }
  return iVar7;
}


RT_GUEST_FN(0x00271310, guest::textGridPrintf)

}  // namespace guest
