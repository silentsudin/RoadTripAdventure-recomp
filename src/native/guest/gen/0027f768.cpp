// guest:0x0027f768 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _vfprintf_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(656);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x1e4) = (uint32_t)(int32_t)param_1;
    *(uint32_t*)rtguest::G(rt_sv+0x288) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x28c) = 0x00000000u;
    *(uint32_t*)rtguest::G(rt_sv+0x1e8) = (uint32_t)(int32_t)param_2;
  }
uint32_t auStack_290 = rtguest::allocFrame(16);
rtguest::allocFrame(4);  // guest frame gap
    uint32_t iStack_27c = rtguest::allocFrame(4);
    uint32_t iStack_278 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t aiStack_270 = rtguest::allocFrame(64);
uint32_t acStack_230 = rtguest::allocFrame(346);
uint32_t acStack_d6 = rtguest::allocFrame(6);
    uint32_t cStack_d0 = rtguest::allocFrame(1);
    uint32_t uStack_cf = rtguest::allocFrame(1);
    rtguest::allocFrame(14);  // guest frame gap
    uint32_t cStack_c0 = rtguest::allocFrame(1);
uint32_t acStack_bf = rtguest::allocFrame(3);
uint32_t aiStack_bc = rtguest::allocFrame(12);
    uint32_t iStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    uint32_t iStack_a8 = rtguest::allocFrame(4);
    uint32_t iStack_a4 = rtguest::allocFrame(4);
    uint32_t uStack_a0 = rtguest::allocFrame(4);
    uint32_t iStack_9c = rtguest::allocFrame(4);
    uint32_t uStack_98 = rtguest::allocFrame(8);
    uint32_t iStack_90 = rtguest::allocFrame(4);
    uint32_t iStack_8c = rtguest::allocFrame(4);
    uint32_t uStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_80 = rtguest::allocFrame(4);
    rtguest::allocFrame(12);  // guest frame gap
    uint32_t uStack_70 = rtguest::allocFrame(4);
    uint32_t uStack_6c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_60 = rtguest::allocFrame(4);
    uint32_t uStack_5c = rtguest::allocFrame(4);
  ushort uVar1;
  bool bVar2;
  int iVar3;
  char cVar4;
uint32_t piVar5;
  int iVar6;
uint32_t pvVar7;
uint32_t puVar8;
  long lVar9;
  long lVar10;
  size_t sVar11;
  ulong uVar12;
  uint uVar13;
  undefined8 uVar14;
  size_t sVar15;
  undefined8 uVar16;
  int iVar17;
  ulong uVar18;
uint32_t pcVar19;
  size_t sVar20;
uint32_t __s;
uint32_t pcVar21;
uint32_t puVar22;
uint32_t puVar23;
  uint uVar24;
uint32_t piStack_280;
uint32_t pcStack_84;
  
  (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = param_1;
  (*(int32_t*)rtguest::G((uint32_t)iStack_a8)) = param_2;
  piVar5 = (uint32_t)RT_CALLRA(0x27f7a0, guest::localeconv());
  (*(int32_t*)rtguest::G((uint32_t)iStack_9c)) = (*(int32_t*)rtguest::G((uint32_t)piVar5));
  uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + 0xc)));
  (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(1) * 4)) = 0;
  if (((uVar1 & 8) == 0) || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + 0x10))) == 0)) {
    lVar9 = RT_CALLRA(0x27f7d0, guest::__swsetup_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8))));
    if (lVar9 != 0) {
      return -1;
    }
    uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + 0xc)));
  }
  piStack_280 = aiStack_270;
  if (((uVar1 & 0x1a) == 10) && (-1 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + 0xe))))) {
    iVar6 = RT_CALLRA(0x27f80c, guest::__sbprintf((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),param_3,param_4));
    return iVar6;
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = 0;
  (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) = 0;
  piVar5 = piStack_280;
  pcVar19 = param_3;
LAB_0027f840:
  lVar9 = RT_CALLRA(0x27f858, guest::_mbtowc_r((*(uint32_t*)rtguest::G(0x002e8cc0)),aiStack_bc,param_3,(*(uint32_t*)rtguest::G(0x2e9128)),aiStack_bc + sizeof(int32_t)));
  if (0 < lVar9) goto code_r0x0027f86c;
  iVar6 = (int)param_3 - (int)pcVar19;
  goto LAB_0027f880;
code_r0x0027f86c:
  param_3 = param_3 + (int)lVar9;
  if ((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(0) * 4)) == 0x25) {
    param_3 = param_3 + -1;
    iVar6 = (int)param_3 - (int)pcVar19;
LAB_0027f880:
    if (iVar6 != 0) {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)pcVar19;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar10 = RT_CALLRA(0x27f8b8, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar10 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) = (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) + iVar6;
    }
    if (0 < lVar9) {
      (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '\0';
      param_3 = param_3 + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = 0;
      uVar13 = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
      sVar20 = 0xffffffffffffffff;
      puVar22 = param_4;
LAB_0027f8fc:
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
LAB_0027f900:
      param_3 = param_3 + 1;
      lVar9 = (long)(int)cVar4;
LAB_0027f90c:
      iVar6 = (int)lVar9;
LAB_0027f910:
      puVar23 = puVar22;
      uVar24 = uVar13;
      pcVar19 = param_3;
      switch(iVar6) {
      case 0x20:
        if ((*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) == '\0') {
          (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = ' ';
          goto LAB_0027f97c;
        }
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
        goto LAB_0027f900;
      default:
        __s = acStack_230;
        if (lVar9 == 0) goto LAB_00280d64;
        (*(int8_t*)rtguest::G((uint32_t)acStack_230 + (uint32_t)(0) * 1)) = (char)lVar9;
        sVar20 = 1;
        param_4 = puVar22;
        break;
      case 0x23:
        uVar13 = uVar13 | 1;
        goto LAB_0027f8fc;
      case 0x2a:
        puVar23 = puVar22 + sizeof(uint64_t);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
        puVar22 = puVar23;
        if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) < 0) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = -(*(uint32_t*)rtguest::G((uint32_t)uStack_a0));
          goto switchD_0027f928_caseD_2d;
        }
        goto LAB_0027f8fc;
      case 0x2b:
        (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '+';
LAB_0027f97c:
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
        goto LAB_0027f900;
      case 0x2d:
switchD_0027f928_caseD_2d:
        uVar13 = uVar13 | 4;
        puVar22 = puVar23;
        goto LAB_0027f8fc;
      case 0x2e:
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
        lVar9 = (long)cVar4;
        param_3 = param_3 + 1;
        if (lVar9 != 0x2a) goto LAB_0027f9b8;
        sVar20 = (size_t)(int)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
        puVar22 = puVar22 + sizeof(uint64_t);
        if ((long)sVar20 < -1) {
          sVar20 = 0xffffffffffffffff;
        }
        goto LAB_0027f8fc;
      case 0x30:
        uVar13 = uVar13 | 0x80;
        goto LAB_0027f8fc;
      case 0x31:
      case 0x32:
      case 0x33:
      case 0x34:
      case 0x35:
      case 0x36:
      case 0x37:
      case 0x38:
      case 0x39:
        goto switchD_0027f928_caseD_31;
      case 0x44:
        uVar13 = uVar13 | 0x10;
      case 100:
      case 0x69:
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 0x40) == 0) {
            uVar18 = (uint32_t)(int)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
          else {
            uVar18 = (uint32_t)(short)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
        }
        else {
          uVar18 = (*(uint64_t*)rtguest::G((uint32_t)puVar22));
        }
        iVar6 = 1;
        if ((long)uVar18 < 0) {
          uVar18 = -uVar18;
          (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '-';
        }
LAB_0027fec0:
        param_4 = puVar22 + sizeof(uint64_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) = (int)sVar20;
        if (-1 < (long)sVar20) {
          uVar13 = uVar13 & 0xffffff7f;
        }
        __s = acStack_d6 + 2;
        uVar24 = uVar13;
        if ((uVar18 == 0) && (puVar8 = auStack_290 + -(int)__s, (int)sVar20 == 0))
        goto LAB_0027ffd4;
        if (iVar6 == 1) goto LAB_0027ff84;
        if (iVar6 == 0) goto LAB_0027ff18;
        if (iVar6 == 2) goto LAB_0027ffa8;
        __s = 0x3321d8u;
        sVar11 = ::strlen((char *)rtguest::G((uint32_t)(0x3321d8u)));
        goto LAB_0027fff4;
      case 0x45:
      case 0x47:
      case 0x65:
      case 0x66:
      case 0x67:
        if (sVar20 == 0xffffffffffffffff) {
          sVar20 = 6;
        }
        else if (((lVar9 == 0x67) || (lVar9 == 0x47)) && (sVar20 == 0)) {
          sVar20 = 1;
        }
        (*(uint64_t*)rtguest::G((uint32_t)uStack_98)) = (*(uint64_t*)rtguest::G((uint32_t)puVar22));
        param_4 = puVar22 + sizeof(uint64_t);
        lVar10 = RT_CALLRA(0x27fb3c, guest::isinf((*(uint64_t*)rtguest::G((uint32_t)uStack_98))));
        if (lVar10 == 0) {
          lVar10 = RT_CALLRA(0x27fb84, guest::isnan((*(uint64_t*)rtguest::G((uint32_t)uStack_98))));
          if (lVar10 == 0) {
            uVar24 = uVar13 | 0x100;
            __s = (uint32_t)RT_CALLRA(0x27fbc0, guest::cvt((*(uint32_t*)rtguest::G((uint32_t)uStack_ac)),(*(uint64_t*)rtguest::G((uint32_t)uStack_98)),sVar20,uVar24,cStack_c0,aiStack_bc + 2 * sizeof(int32_t),lVar9,
                              iStack_b0));
            if ((lVar9 == 0x67) || (lVar9 == 0x47)) {
              if (((long)(*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) < -3) || ((long)sVar20 < (long)(*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)))) {
                bVar2 = lVar9 != 0x67;
                lVar9 = 0x65;
                if (bVar2) {
                  lVar9 = 0x45;
                }
              }
              else {
                lVar9 = 0x67;
              }
            }
            sVar15 = (size_t)(*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
            if (lVar9 < 0x66) {
              (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) = (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) + -1;
              (*(int32_t*)rtguest::G((uint32_t)iStack_90)) = RT_CALLRA(0x27fc2c, guest::exponent(auStack_290,(*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)),lVar9));
              if ((1 < (*(int32_t*)rtguest::G((uint32_t)iStack_b0))) || (sVar11 = (long)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0))), (uVar13 & 1) != 0)) {
                sVar11 = (size_t)((*(int32_t*)rtguest::G((uint32_t)iStack_90)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + 1);
              }
            }
            else if (lVar9 == 0x66) {
              sVar11 = (long)((int)sVar20 + 2);
              if ((0 < (long)sVar15) && ((sVar20 != 0 || (sVar11 = sVar15, (uVar13 & 1) != 0)))) {
                sVar11 = (long)((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) + (int)sVar20 + 1);
              }
            }
            else if ((long)sVar15 < (long)(*(int32_t*)rtguest::G((uint32_t)iStack_b0))) {
              if ((long)sVar15 < 1) {
                sVar11 = (size_t)(((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) - (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4))) + 2);
              }
              else {
                sVar11 = (size_t)((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + 1);
              }
            }
            else {
              sVar11 = sVar15;
              if ((uVar13 & 1) != 0) {
                sVar11 = (size_t)((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) + 1);
              }
            }
            if ((*(int8_t*)rtguest::G((uint32_t)cStack_c0)) != '\0') {
              (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '-';
            }
          }
          else {
            sVar11 = 3;
            __s = 0x332198u;
          }
        }
        else {
          lVar10 = RT_CALLRA(0x27fb54, guest::dpcmp((*(uint64_t*)rtguest::G((uint32_t)uStack_98)),0));
          if (lVar10 < 0) {
            (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '-';
          }
          sVar11 = 3;
          __s = 0x332190u;
        }
        goto LAB_0027fff4;
      case 0x4c:
        uVar13 = uVar13 | 8;
        goto LAB_0027f8fc;
      case 0x4f:
        uVar13 = uVar13 | 0x10;
      case 0x6f:
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 0x40) == 0) {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
          else {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
        }
        else {
          uVar18 = (*(uint64_t*)rtguest::G((uint32_t)puVar22));
        }
        iVar6 = 0;
LAB_0027febc:
        (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '\0';
        goto LAB_0027fec0;
      case 0x55:
        uVar13 = uVar13 | 0x10;
      case 0x75:
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 0x40) == 0) {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
          else {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
        }
        else {
          uVar18 = (*(uint64_t*)rtguest::G((uint32_t)puVar22));
        }
        iVar6 = 1;
        goto LAB_0027febc;
      case 0x58:
        pcStack_84 = 0x3321c0u;
        goto LAB_0027fe68;
      case 99:
        param_4 = puVar22 + sizeof(uint64_t);
        __s = acStack_230;
        (*(int8_t*)rtguest::G((uint32_t)acStack_230 + (uint32_t)(0) * 1)) = (char)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
        sVar20 = 1;
        break;
      case 0x68:
        uVar13 = uVar13 | 0x40;
        goto LAB_0027f8fc;
      case 0x6c:
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
        if ((*(int8_t*)rtguest::G((uint32_t)param_3)) == 'l') {
          param_3 = param_3 + 1;
          goto switchD_0027f928_caseD_71;
        }
        uVar13 = uVar13 | 0x10;
        goto LAB_0027f900;
      case 0x6e:
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 0x40) == 0) {
            param_4 = puVar22 + sizeof(uint64_t);
            (*(int32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar22)))) = (*(int32_t*)rtguest::G((uint32_t)iStack_a4));
          }
          else {
            param_4 = puVar22 + sizeof(uint64_t);
            (*(uint16_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar22)))) = (short)(*(int32_t*)rtguest::G((uint32_t)iStack_a4));
          }
        }
        else {
          param_4 = puVar22 + sizeof(uint64_t);
          (*(int64_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar22)))) = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_a4));
        }
        goto LAB_0027f840;
      case 0x70:
        uVar18 = (uint32_t)(int)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
        uVar13 = uVar13 | 2;
        pcStack_84 = 0x3321a0u;
        iVar6 = 2;
        lVar9 = 0x78;
        goto LAB_0027febc;
      case 0x71:
switchD_0027f928_caseD_71:
        uVar13 = uVar13 | 0x20;
        goto LAB_0027f8fc;
      case 0x73:
        __s = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)puVar22));
        param_4 = puVar22 + sizeof(uint64_t);
        if (__s == (uint32_t)0x0) {
          __s = 0x3321b8u;
        }
        if ((long)sVar20 < 0) {
          sVar20 = ::strlen((char *)rtguest::G((uint32_t)(__s)));
        }
        else {
          pvVar7 = RT_CALLRA(0x27fdcc, guest::memchr(__s,0,sVar20));
          if ((pvVar7 != (uint32_t)0x0) &&
             (sVar11 = (size_t)((int)pvVar7 - (int)__s), (long)sVar11 <= (long)sVar20)) {
            (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '\0';
            goto LAB_0027fff4;
          }
        }
        break;
      case 0x78:
        pcStack_84 = 0x3321a0u;
LAB_0027fe68:
        if ((uVar13 & 0x10) == 0) {
          if ((uVar13 & 0x40) == 0) {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
          else {
            uVar18 = (uint32_t)(uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar22));
          }
        }
        else {
          uVar18 = (*(uint64_t*)rtguest::G((uint32_t)puVar22));
        }
        iVar6 = 2;
        if (((uVar13 & 1) != 0) && (uVar18 != 0)) {
          uVar13 = uVar13 | 2;
        }
        goto LAB_0027febc;
      }
      sVar11 = sVar20;
      (*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) = '\0';
      goto LAB_0027fff4;
    }
LAB_00280d64:
    if ((*(int32_t*)rtguest::G((uint32_t)iStack_278)) != 0) {
      RT_CALLRA(0x2800a4, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
    }
LAB_00280d84:
    iVar6 = -1;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)iStack_a8)) + 0xc))) & 0x40) == 0) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_a4));
    }
    return iVar6;
  }
  goto LAB_0027f840;
LAB_0027ff84:
  for (; 9 < uVar18; uVar18 = __udivdi3(uVar18,10)) {
    cVar4 = RT_CALLRA(0x27ff58, guest::__umoddi3(uVar18,10));
    __s = __s + -1;
    (*(int8_t*)rtguest::G((uint32_t)__s)) = cVar4 + '0';
  }
  __s = __s + -1;
  (*(int8_t*)rtguest::G((uint32_t)__s)) = (char)uVar18 + '0';
LAB_0027ffd0:
  puVar8 = auStack_290 + -(int)__s;
LAB_0027ffd4:
  sVar11 = (size_t)(int)(puVar8 + 0x1bc);
LAB_0027fff4:
  sVar20 = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_8c));
  if ((long)(*(int32_t*)rtguest::G((uint32_t)iStack_8c)) <= (long)sVar11) {
    sVar20 = sVar11;
  }
  iVar6 = (int)sVar11;
  if ((*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) == '\0') {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (uVar24 & 2) + (int)sVar20;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_88)) = (int)sVar20 + 1;
  }
  (*(uint32_t*)rtguest::G((uint32_t)uStack_80)) = uVar24 & 0x84;
  if ((uVar24 & 0x84) == 0) {
    iVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) - (*(uint32_t*)rtguest::G((uint32_t)uStack_88));
    if (0 < iVar17) {
      uVar16 = 0x330000;
      if (iVar17 < 0x11) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
      }
      else {
        uVar14 = 0x10;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
        while( true ) {
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332170u;
          piVar5 = piVar5 + 2 * sizeof(int32_t);
          (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
          (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
          if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
            (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = (uint32_t)uVar14;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)((uint64_t)uVar14 >> 0x20);
            (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
            lVar10 = RT_CALLRA(0x28010c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
            uVar14 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_70)));
            uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
            if (lVar10 != 0) goto LAB_00280d84;
            piVar5 = aiStack_270;
          }
          iVar17 = iVar17 + -0x10;
          if (iVar17 < 0x11) break;
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (int)uVar14;
        }
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2170;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar17;
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar10 = RT_CALLRA(0x280164, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar10 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
    }
  }
  if ((*(int8_t*)rtguest::G((uint32_t)acStack_bf + (uint32_t)(0) * 1)) == '\0') {
    if ((uVar24 & 2) != 0) {
      (*(int8_t*)rtguest::G((uint32_t)cStack_d0)) = '0';
      (*(uint8_t*)rtguest::G((uint32_t)uStack_cf)) = (char)lVar9;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 2;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)cStack_d0;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 2;
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar10 = RT_CALLRA(0x2801cc, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar10 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
    }
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)acStack_bf;
    piVar5 = piVar5 + 2 * sizeof(int32_t);
    (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
    (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
    if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
      lVar10 = RT_CALLRA(0x280254, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
      if (lVar10 != 0) goto LAB_00280d84;
      piVar5 = aiStack_270;
    }
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)uStack_80)) == 0x80) && (iVar17 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) - (*(uint32_t*)rtguest::G((uint32_t)uStack_88)), 0 < iVar17)) {
    uVar16 = 0x330000;
    if (iVar17 < 0x11) {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
    }
    else {
      uVar14 = 0x10;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      while( true ) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = (uint32_t)uVar14;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)((uint64_t)uVar14 >> 0x20);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
          lVar10 = RT_CALLRA(0x2802bc, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          uVar14 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_70)));
          uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
          if (lVar10 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        iVar17 = iVar17 + -0x10;
        if (iVar17 < 0x11) break;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (int)uVar14;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
    piVar5 = piVar5 + 2 * sizeof(int32_t);
    (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
    (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar17;
    if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
      lVar10 = RT_CALLRA(0x280334, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
      if (lVar10 != 0) goto LAB_00280d84;
      piVar5 = aiStack_270;
    }
  }
  iVar17 = (*(int32_t*)rtguest::G((uint32_t)iStack_8c)) - iVar6;
  if (0 < iVar17) {
    uVar16 = 0x330000;
    if (iVar17 < 0x11) {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
    }
    else {
      uVar14 = 0x10;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      while( true ) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = (uint32_t)uVar14;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = (uint32_t)((uint64_t)uVar14 >> 0x20);
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
          lVar10 = RT_CALLRA(0x28039c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          uVar14 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_6c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_70)));
          uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
          if (lVar10 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        iVar17 = iVar17 + -0x10;
        if (iVar17 < 0x11) break;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (int)uVar14;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar17;
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
    piVar5 = piVar5 + 2 * sizeof(int32_t);
    (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
    (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar17;
    if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
      lVar10 = RT_CALLRA(0x28043c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
      if (lVar10 != 0) goto LAB_00280d84;
      piVar5 = aiStack_270;
    }
  }
  if ((uVar24 & 0x100) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)__s;
    (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
LAB_00280c08:
    piVar5 = piVar5 + 2 * sizeof(int32_t);
    (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
    bVar2 = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) < 8;
  }
  else {
    if (lVar9 < 0x66) {
      if ((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) < 2) {
        if ((uVar24 & 1) != 0) {
          (*(int8_t*)rtguest::G((uint32_t)cStack_d0)) = (*(int8_t*)rtguest::G((uint32_t)__s));
          goto LAB_002809f4;
        }
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)__s;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x2804a8, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
      }
      else {
        (*(int8_t*)rtguest::G((uint32_t)cStack_d0)) = (*(int8_t*)rtguest::G((uint32_t)__s));
LAB_002809f4:
        (*(uint8_t*)rtguest::G((uint32_t)uStack_cf)) = 0x2e;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 2;
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)cStack_d0;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 2;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280518, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        lVar9 = RT_CALLRA(0x2803f0, guest::dpcmp((*(uint64_t*)rtguest::G((uint32_t)uStack_98)),0));
        if (lVar9 == 0) {
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + -1;
          if (0 < iVar6) {
            uVar16 = 0x330000;
            if (iVar6 < 0x11) {
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
            }
            else {
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
              while( true ) {
                (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
                piVar5 = piVar5 + 2 * sizeof(int32_t);
                (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
                (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
                if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
                  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
                  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
                  lVar9 = RT_CALLRA(0x2805bc, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
                  uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
                  if (lVar9 != 0) goto LAB_00280d84;
                  piVar5 = aiStack_270;
                }
                iVar6 = iVar6 + -0x10;
                if (iVar6 < 0x11) break;
                (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
              }
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
            }
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
            piVar5 = piVar5 + 2 * sizeof(int32_t);
            (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
            (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
            if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
              lVar9 = RT_CALLRA(0x280614, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
              if (lVar9 != 0) goto LAB_00280d84;
              piVar5 = aiStack_270;
            }
          }
        }
        else {
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)(__s + 1);
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + -1;
          piVar5 = piVar5 + 2 * sizeof(int32_t);
          (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
          (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + -1;
          if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
            lVar9 = RT_CALLRA(0x280688, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
            if (lVar9 != 0) goto LAB_00280d84;
            piVar5 = aiStack_270;
          }
        }
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_90));
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)auStack_290;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
      goto LAB_00280c08;
    }
    lVar9 = RT_CALLRA(0x280a5c, guest::dpcmp((*(uint64_t*)rtguest::G((uint32_t)uStack_98)),0));
    if (lVar9 == 0) {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x003321f8;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar9 = RT_CALLRA(0x2806ec, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar9 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) < (*(int32_t*)rtguest::G((uint32_t)iStack_b0))) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
      }
      else {
        if ((uVar24 & 1) == 0) goto LAB_00280c3c;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
      }
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)iStack_9c));
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar9 = RT_CALLRA(0x28073c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar9 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_b0)) + -1;
      if (iVar6 < 1) goto LAB_00280c3c;
      uVar16 = 0x330000;
      if (iVar6 < 0x11) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
        while( true ) {
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
          piVar5 = piVar5 + 2 * sizeof(int32_t);
          (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
          (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
          if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
            (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
            (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
            lVar9 = RT_CALLRA(0x280798, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
            uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
            if (lVar9 != 0) goto LAB_00280d84;
            piVar5 = aiStack_270;
          }
          iVar6 = iVar6 + -0x10;
          if (iVar6 < 0x11) break;
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
        }
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
      bVar2 = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) < 8;
    }
    else {
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) < 1) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x003321f8;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280810, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        if ((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) != 0) {
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
          (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (*(int32_t*)rtguest::G((uint32_t)iStack_9c));
          piVar5 = piVar5 + 2 * sizeof(int32_t);
          (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
          if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
            lVar9 = RT_CALLRA(0x280874, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
            if (lVar9 != 0) goto LAB_00280d84;
            piVar5 = aiStack_270;
          }
          iVar6 = -(*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
          if (0 < iVar6) {
            uVar16 = 0x330000;
            if (iVar6 < 0x11) {
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
            }
            else {
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
              while( true ) {
                (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
                piVar5 = piVar5 + 2 * sizeof(int32_t);
                (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
                (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
                if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
                  (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
                  (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
                  lVar9 = RT_CALLRA(0x280900, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
                  uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
                  if (lVar9 != 0) goto LAB_00280d84;
                  piVar5 = aiStack_270;
                }
                iVar6 = iVar6 + -0x10;
                if (iVar6 < 0x11) break;
                (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
              }
              (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
            }
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
            piVar5 = piVar5 + 2 * sizeof(int32_t);
            (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
            (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
            if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
              lVar9 = RT_CALLRA(0x28095c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
              if (lVar9 != 0) goto LAB_00280d84;
              piVar5 = aiStack_270;
            }
          }
          iVar6 = (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
          (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)__s;
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
          piVar5 = piVar5 + 2 * sizeof(int32_t);
          (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
          (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
          if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
            lVar9 = RT_CALLRA(0x2809bc, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
            if (lVar9 != 0) goto LAB_00280d84;
            piVar5 = aiStack_270;
          }
        }
        goto LAB_00280c3c;
      }
      if ((*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) < (*(int32_t*)rtguest::G((uint32_t)iStack_b0))) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)__s;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280a3c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        iVar6 = (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x00332200;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280aa8, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
        iVar17 = (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4));
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)(__s + iVar6);
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar3 - iVar17;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + ((*(int32_t*)rtguest::G((uint32_t)iStack_b0)) - (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)));
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280b20, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        goto LAB_00280c3c;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)__s;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
      if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
        lVar9 = RT_CALLRA(0x280b84, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
        if (lVar9 != 0) goto LAB_00280d84;
        piVar5 = aiStack_270;
      }
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)aiStack_bc + (uint32_t)(2) * 4)) - (*(int32_t*)rtguest::G((uint32_t)iStack_b0));
      if (0 < iVar6) {
        uVar16 = 0x330000;
        if (iVar6 < 0x11) {
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
        }
        else {
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
          while( true ) {
            (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332180u;
            piVar5 = piVar5 + 2 * sizeof(int32_t);
            (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
            (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
            if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
              (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
              (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
              lVar9 = RT_CALLRA(0x280bd0, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
              uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
              if (lVar9 != 0) goto LAB_00280d84;
              piVar5 = aiStack_270;
            }
            iVar6 = iVar6 + -0x10;
            if (iVar6 < 0x11) break;
            (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
          }
          (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
        }
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2180;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          lVar9 = RT_CALLRA(0x280c20, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
      }
      if ((uVar24 & 1) == 0) goto LAB_00280c3c;
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 1;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x00332200;
      piVar5 = piVar5 + 2 * sizeof(int32_t);
      (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
      (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 1;
      bVar2 = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) < 8;
    }
  }
  if (!bVar2) {
    lVar9 = RT_CALLRA(0x280ca0, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
    if (lVar9 != 0) goto LAB_00280d84;
    piVar5 = aiStack_270;
  }
LAB_00280c3c:
  if (((uVar24 & 4) != 0) && (iVar6 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) - (*(uint32_t*)rtguest::G((uint32_t)uStack_88)), 0 < iVar6)) {
    uVar16 = 0x330000;
    if (iVar6 < 0x11) {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      while( true ) {
        (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)0x332170u;
        piVar5 = piVar5 + 2 * sizeof(int32_t);
        (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
        (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + 0x10;
        if (7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) {
          (*(uint32_t*)rtguest::G((uint32_t)uStack_60)) = (uint32_t)uVar16;
          (*(uint32_t*)rtguest::G((uint32_t)uStack_5c)) = (uint32_t)((uint64_t)uVar16 >> 0x20);
          lVar9 = RT_CALLRA(0x280d00, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20)));
          uVar16 = CONCAT44((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),(*(uint32_t*)rtguest::G((uint32_t)uStack_60)));
          if (lVar9 != 0) goto LAB_00280d84;
          piVar5 = aiStack_270;
        }
        iVar6 = iVar6 + -0x10;
        if (iVar6 < 0x11) break;
        (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = 0x10;
      }
      (*(int32_t*)rtguest::G((uint32_t)piVar5 + (uint32_t)(1) * sizeof(int32_t))) = iVar6;
    }
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = (int)uVar16 + 0x2170;
    (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) + 1;
    (*(int32_t*)rtguest::G((uint32_t)iStack_278)) = (*(int32_t*)rtguest::G((uint32_t)iStack_278)) + iVar6;
    if ((7 < (*(int32_t*)rtguest::G((uint32_t)iStack_27c))) && (lVar9 = RT_CALLRA(0x280d3c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20))), lVar9 != 0))
    goto LAB_00280d84;
  }
  uVar13 = (*(uint32_t*)rtguest::G((uint32_t)uStack_a0));
  if ((int)(*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) <= (int)(*(uint32_t*)rtguest::G((uint32_t)uStack_88))) {
    uVar13 = (*(uint32_t*)rtguest::G((uint32_t)uStack_88));
  }
  (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) = (*(int32_t*)rtguest::G((uint32_t)iStack_a4)) + uVar13;
  if (((*(int32_t*)rtguest::G((uint32_t)iStack_278)) != 0) && (lVar9 = RT_CALLRA(0x280d6c, guest::__sprint_r((*(int32_t*)rtguest::G((uint32_t)iStack_a8)),rtguest::allocFrame(0x20))), lVar9 != 0))
  goto LAB_00280d84;
  piVar5 = aiStack_270;
  (*(int32_t*)rtguest::G((uint32_t)iStack_27c)) = 0;
  goto LAB_0027f840;
LAB_0027ff18:
  do {
    pcVar21 = __s;
    uVar12 = uVar18 & 7;
    uVar18 = uVar18 >> 3;
    lVar10 = uVar12 + 0x30;
    __s = pcVar21 + -1;
    (*(int8_t*)rtguest::G((uint32_t)__s)) = (char)lVar10;
  } while (uVar18 != 0);
  if ((uVar13 & 1) == 0) goto LAB_0027ffd0;
  if (lVar10 != 0x30) {
    __s = pcVar21 + -2;
    (*(int8_t*)rtguest::G((uint32_t)__s)) = '0';
    goto LAB_0027ffd0;
  }
  puVar8 = auStack_290 + -(int)__s;
  goto LAB_0027ffd4;
LAB_0027ffa8:
  do {
    uVar13 = (uint32_t)uVar18;
    __s = __s + -1;
    uVar18 = uVar18 >> 4;
    (*(int8_t*)rtguest::G((uint32_t)__s)) = (*(int8_t*)rtguest::G((uint32_t)pcStack_84 + (uint32_t)(uVar13 & 0xf) * sizeof(int8_t)));
  } while (uVar18 != 0);
  goto LAB_0027ffd0;
switchD_0027f928_caseD_31:
  (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = 0;
  do {
    iVar6 = (int)lVar9;
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
    lVar9 = (long)cVar4;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) = ((*(uint32_t*)rtguest::G((uint32_t)uStack_a0)) * 10 + iVar6) - 0x30;
    param_3 = param_3 + 1;
  } while ((int)cVar4 - 0x30U < 10);
  goto LAB_0027f90c;
LAB_0027f9b8:
  sVar20 = 0;
  while (iVar6 = (int)lVar9, (int)cVar4 - 0x30U < 10) {
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)param_3));
    lVar9 = (long)cVar4;
    sVar20 = (size_t)((int)sVar20 * 10 + iVar6 + -0x30);
    param_3 = param_3 + 1;
  }
  if ((long)sVar20 < -1) goto code_r0x0027f9f8;
  goto LAB_0027f910;
code_r0x0027f9f8:
  sVar20 = 0xffffffffffffffff;
  goto LAB_0027f90c;
}


RT_GUEST_FN(0x0027f768, guest::_vfprintf_r)

}  // namespace guest
