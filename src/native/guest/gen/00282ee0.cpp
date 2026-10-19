// guest:0x00282ee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
// __sfvwrite_r
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sfvwrite_r(uint32_t param_1, uint32_t param_2, ...) {
  uint32_t piVar1;
  uint32_t pcVar2;
  bool bVar3;
  int iVar4;
  uint32_t pvVar5;
  int iVar6;
  long lVar7;
  ushort uVar8;
  uint32_t pcVar9;
  int iVar10;
  uint32_t pcVar11;
  ulong uVar12;
  ulong __n_00;
  uint32_t unaff_s3_lo;
  uint32_t puVar13;
  ulong unaff_s5;
  size_t __n;

  // Ghidra FILE-type offsets as in 002826c0; uio = {iov, , resid} at +0/+4/+8
  if ((*(int32_t*)rtguest::G(param_2 + 8)) != 0) {
    uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
    if (((uVar8 & 8) == 0) || ((*(int32_t*)rtguest::G(param_1 + 0x10)) == 0)) {
      lVar7 = __swsetup_r(param_1);  // arg-drop fix: a0 live at jal 0x282f38 (disasm)
      if (lVar7 != 0) {
        return 0xffffffff;
      }
      uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
    }
    puVar13 = (*(uint32_t*)rtguest::G(param_2));
    __n_00 = 0;
    if ((uVar8 & 2) == 0) {
      bVar3 = false;
      if ((uVar8 & 1) == 0) {
joined_r0x00282ff0:
        while (__n_00 == 0) {
          piVar1 = puVar13 + 4;
          unaff_s3_lo = (*(uint32_t*)rtguest::G(puVar13));
          puVar13 = puVar13 + 8;
          __n_00 = (ulong)(*(int32_t*)rtguest::G(piVar1));
        }
        pcVar9 = (*(uint32_t*)rtguest::G(param_1 + 8));
        uVar12 = (ulong)(int)pcVar9;
        iVar6 = (int)__n_00;
        if ((uVar8 & 0x200) == 0) {
          pcVar2 = (*(uint32_t*)rtguest::G(param_1));
          if ((*(uint32_t*)rtguest::G(param_1 + 0x10)) < pcVar2) {
            if (__n_00 <= uVar12) {
              pcVar9 = (*(uint32_t*)rtguest::G(param_1 + 0x14));
              goto LAB_002830a0;
            }
            memmove(pcVar2,unaff_s3_lo,uVar12);
            (*(uint32_t*)rtguest::G(param_1)) = (uint32_t)(pcVar9 + (*(uint32_t*)rtguest::G(param_1)));
            iVar4 = fflush(param_1);
            if (iVar4 != 0) {
              uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
              goto LAB_00283288;
            }
            iVar4 = (*(int32_t*)rtguest::G(param_2 + 8));
          }
          else {
            pcVar9 = (*(uint32_t*)rtguest::G(param_1 + 0x14));
LAB_002830a0:
            if (__n_00 < (ulong)(long)(int)pcVar9) {
              memmove(pcVar2,unaff_s3_lo,__n_00);
              pcVar9 = (*(uint32_t*)rtguest::G(param_1 + 8)) - iVar6;
              iVar4 = (*(int32_t*)rtguest::G(param_1)) + iVar6;
              goto LAB_002830f4;
            }
            uVar12 = rtCall((*(uint32_t*)rtguest::G(param_1 + 0x24)),(*(uint32_t*)rtguest::G(param_1 + 0x1c)),unaff_s3_lo,(long)(int)pcVar9);
            if ((long)uVar12 < 1) {
              uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
              goto LAB_00283288;
            }
            iVar4 = (*(int32_t*)rtguest::G(param_2 + 8));
          }
        }
        else {
          if (__n_00 < uVar12) {
            uVar12 = __n_00;
          }
          memmove((*(uint32_t*)rtguest::G(param_1)),unaff_s3_lo,uVar12);
          pcVar9 = (*(uint32_t*)rtguest::G(param_1 + 8)) - (int)uVar12;
          iVar4 = (*(int32_t*)rtguest::G(param_1)) + (int)uVar12;
LAB_002830f4:
          (*(uint32_t*)rtguest::G(param_1 + 8)) = pcVar9;
          (*(uint32_t*)rtguest::G(param_1)) = iVar4;
          iVar4 = (*(int32_t*)rtguest::G(param_2 + 8));
          uVar12 = __n_00;
        }
        iVar10 = (int)uVar12;
        unaff_s3_lo = unaff_s3_lo + iVar10;
        __n_00 = (ulong)(iVar6 - iVar10);
        (*(int32_t*)rtguest::G(param_2 + 8)) = iVar4 - iVar10;
        if (iVar4 - iVar10 != 0) {
          uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
          goto joined_r0x00282ff0;
        }
      }
      else {
        do {
          if (__n_00 == 0) {
            bVar3 = false;
            do {
              __n_00 = (ulong)(int)(*(int32_t*)rtguest::G(puVar13 + 4));
              unaff_s3_lo = (*(uint32_t*)rtguest::G(puVar13));
              puVar13 = puVar13 + 8;
            } while (__n_00 == 0);
          }
          if (bVar3) {
            pcVar9 = (*(uint32_t*)rtguest::G(param_1));
          }
          else {
            pvVar5 = memchr(unaff_s3_lo,10,__n_00);
            iVar6 = (int)__n_00;
            if (pvVar5 != (uint32_t)0x0) {
              iVar6 = (int)pvVar5 - (int)unaff_s3_lo;
            }
            unaff_s5 = (ulong)(iVar6 + 1);
            bVar3 = true;
            pcVar9 = (*(uint32_t*)rtguest::G(param_1));
          }
          uVar12 = __n_00;
          if (unaff_s5 <= __n_00) {
            uVar12 = unaff_s5;
          }
          pcVar2 = (*(uint32_t*)rtguest::G(param_1 + 0x14));
          pcVar11 = (*(uint32_t*)rtguest::G(param_1 + 8)) + (int)pcVar2;
          __n = (size_t)(int)pcVar11;
          iVar6 = (int)unaff_s5;
          if (((*(uint32_t*)rtguest::G(param_1 + 0x10)) < pcVar9) && ((long)__n < (long)uVar12)) {
            memmove(pcVar9,unaff_s3_lo,__n);
            (*(uint32_t*)rtguest::G(param_1)) = (uint32_t)(pcVar11 + (*(uint32_t*)rtguest::G(param_1)));
            iVar4 = fflush(param_1);
            unaff_s5 = (ulong)(iVar6 - (int)pcVar11);
            if (iVar4 != 0) {
              uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
              goto LAB_00283288;
            }
          }
          else if ((long)uVar12 < (long)(int)pcVar2) {
            memmove(pcVar9,unaff_s3_lo,uVar12);
            iVar4 = (int)uVar12;
            (*(uint32_t*)rtguest::G(param_1 + 8)) = (*(uint32_t*)rtguest::G(param_1 + 8)) - iVar4;
            (*(uint32_t*)rtguest::G(param_1)) = (*(uint32_t*)rtguest::G(param_1)) + iVar4;
            unaff_s5 = (ulong)(iVar6 - iVar4);
            __n = uVar12;
          }
          else {
            __n = rtCall((*(uint32_t*)rtguest::G(param_1 + 0x24)),(*(uint32_t*)rtguest::G(param_1 + 0x1c)),unaff_s3_lo,(long)(int)pcVar2);
            unaff_s5 = (ulong)(iVar6 - (int)__n);
            if ((long)__n < 1) {
              uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
              goto LAB_00283288;
            }
          }
          if (unaff_s5 == 0) {
            iVar6 = fflush(param_1);
            if (iVar6 != 0) {
              uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
              goto LAB_00283288;
            }
            bVar3 = false;
            iVar6 = (*(int32_t*)rtguest::G(param_2 + 8));
          }
          else {
            iVar6 = (*(int32_t*)rtguest::G(param_2 + 8));
          }
          iVar4 = (int)__n;
          unaff_s3_lo = unaff_s3_lo + iVar4;
          __n_00 = (ulong)((int)__n_00 - iVar4);
          (*(int32_t*)rtguest::G(param_2 + 8)) = iVar6 - iVar4;
        } while (iVar6 - iVar4 != 0);
      }
    }
    else {
      do {
        while (__n_00 == 0) {
          piVar1 = puVar13 + 4;
          unaff_s3_lo = (*(uint32_t*)rtguest::G(puVar13));
          puVar13 = puVar13 + 8;
          __n_00 = (ulong)(*(int32_t*)rtguest::G(piVar1));
        }
        uVar12 = 0x400;
        if (__n_00 < 0x401) {
          uVar12 = __n_00;
        }
        lVar7 = (long)rtCall((*(uint32_t*)rtguest::G(param_1 + 0x24)),(*(uint32_t*)rtguest::G(param_1 + 0x1c)),unaff_s3_lo,uVar12);
        if (lVar7 < 1) {
          uVar8 = (*(uint16_t*)rtguest::G(param_1 + 0xc));
LAB_00283288:
          (*(uint16_t*)rtguest::G(param_1 + 0xc)) = uVar8 | 0x40;
          return 0xffffffff;
        }
        iVar6 = (*(int32_t*)rtguest::G(param_2 + 8));
        iVar4 = (int)lVar7;
        unaff_s3_lo = unaff_s3_lo + iVar4;
        __n_00 = (ulong)((int)__n_00 - iVar4);
        (*(int32_t*)rtguest::G(param_2 + 8)) = iVar6 - iVar4;
      } while (iVar6 - iVar4 != 0);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00282ee0, guest::__sfvwrite_r)

}  // namespace guest
