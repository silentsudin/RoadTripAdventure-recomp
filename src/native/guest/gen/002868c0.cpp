// guest:0x002868c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _printf(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
  }
  byte bVar1;
uint32_t pbVar2;
  char cVar3;
  char cVar4;
  undefined8 uVar5;
  uint uVar6;
  long lVar7;
uint32_t pcVar8;
uint32_t pcVar9;
  ulong uVar10;
uint32_t pfVar11;
uint32_t acStack_c0 = rtguest::allocFrame(80);
  
LAB_00286e58:
  while( true ) {
    if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == 0) {
      return 0;
    }
    pcVar9 = (uint32_t)0x0;
    lVar7 = 0;
    if ((*(int8_t*)rtguest::G((uint32_t)param_1)) == 0x25) break;
    param_1 = param_1 + 1;
    RT_CALLRA(0x00286a7c, rtCall((*(uint32_t*)rtguest::G(0x002e98f8))));
  }
LAB_00286910:
  pbVar2 = param_1;
  param_1 = pbVar2 + 1;
  switch((int)(((*(int8_t*)rtguest::G((uint32_t)param_1)) - 0x30) * 0x1000000) >> 0x18) {
  case 0:
    uVar6 = (int)(char)(*(int8_t*)rtguest::G((uint32_t)pbVar2 + (uint32_t)(2) * sizeof(int8_t))) - 0x30;
    if ((uVar6 & 0xff) < 10) {
      if ((int)(char)(*(int8_t*)rtguest::G((uint32_t)pbVar2 + (uint32_t)(3) * sizeof(int8_t))) - 0x30U < 10) {
        param_1 = pbVar2 + 3;
        uVar6 = uVar6 * 10 + -0x30 + (int)(char)(*(int8_t*)rtguest::G((uint32_t)pbVar2 + (uint32_t)(3) * sizeof(int8_t)));
        if (0x1f < (int)uVar6) {
          uVar6 = 0x1f;
        }
      }
      else {
        param_1 = pbVar2 + 2;
      }
      pcVar9 = acStack_c0 + (0x4f - uVar6);
      for (; 0 < (int)uVar6; uVar6 = uVar6 - 1) {
        (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4f - uVar6) * 1)) = '0';
      }
    }
    goto LAB_00286910;
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
  case 0x37:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3d:
  case 0x3e:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x44:
  case 0x46:
  case 0x47:
    goto switchD_0028693c_caseD_1;
  case 0x33:
    param_1 = pbVar2 + 2;
    RT_CALLRA(0x00286b3c, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)param_2))));
    param_2 = param_2 + 8;
    goto LAB_00286e58;
  case 0x34:
    if (lVar7 == 0x6c) {
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else if (lVar7 == 0x68) {
      uVar10 = (uint64_t)(short)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else {
      // MIPS reads the va_list slot with a signed lw (Ghidra typed the pointer float*)
      uVar10 = (uint64_t)(*(int32_t*)rtguest::G((uint32_t)param_2));
    }
    param_2 = param_2 + 8;
    pcVar8 = acStack_c0 + 0x4f;
    (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4f) * 1)) = 0;
    if (uVar10 == 0) {
      pcVar8 = acStack_c0 + 0x4e;
      (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4e) * 1)) = 0x30;
    }
    else {
      if ((long)uVar10 < 0) {
        uVar10 = -uVar10;
        RT_CALLRA(0x00286bb0, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x2d));
      }
      for (; uVar10 != 0; uVar10 = __divdi3(uVar10,10)) {
        cVar4 = RT_CALLRA(0x286bc8, guest::__moddi3(uVar10,10));
        pcVar8 = pcVar8 + -1;
        (*(int8_t*)rtguest::G((uint32_t)pcVar8)) = cVar4 + '0';
      }
    }
    param_1 = pbVar2 + 2;
    if ((pcVar9 != (uint32_t)0x0) && (pcVar9 < pcVar8)) {
      pcVar8 = pcVar9;
    }
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) == '\0') break;
    do {
      pcVar8 = pcVar8 + 1;
      RT_CALLRA(0x00286c2c, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), cVar4));
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    } while ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0');
    goto LAB_00286e58;
  case 0x35:
  case 0x36:
    if ((*(float*)rtguest::G((uint32_t)param_2)) == 0.0) {
      param_1 = pbVar2 + 2;
      RT_CALLRA(0x00286cfc, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x30));
      param_2 = param_2 + 8;
    }
    else {
      param_1 = pbVar2 + 2;
      // MIPS leaves $4 = 0x286d18 (this case's jump-table target) at the jal; Ghidra dropped it
      uVar5 = RT_CALLRA(0x286d4c, guest::fptodp(0x286d18));
      RT_CALLRA(0x286d54, guest::fpFormatFloatSci(uVar5));
      param_2 = param_2 + 8;
    }
    goto LAB_00286e58;
  case 0x38:
    lVar7 = 0x68;
    goto LAB_00286910;
  case 0x3c:
    goto switchD_0028693c_caseD_3c;
  case 0x3f:
    if (lVar7 == 0x6c) {
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else if (lVar7 == 0x68) {
      uVar10 = (uint64_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else {
      // MIPS reads the va_list slot with an unsigned lwu (Ghidra typed the pointer float*)
      uVar10 = (uint64_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
    }
    param_2 = param_2 + 8;
    pcVar8 = acStack_c0 + 0x4f;
    (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4f) * 1)) = 0;
    if (uVar10 == 0) {
      pcVar8 = acStack_c0 + 0x4e;
      (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4e) * 1)) = 0x30;
    }
    else {
      do {
        bVar1 = (uint32_t)uVar10;
        pcVar8 = pcVar8 + -1;
        uVar10 = uVar10 >> 3;
        (*(int8_t*)rtguest::G((uint32_t)pcVar8)) = (bVar1 & 7) + 0x30;
      } while (uVar10 != 0);
    }
    param_1 = pbVar2 + 2;
    if ((pcVar9 != (uint32_t)0x0) && (pcVar9 < pcVar8)) {
      pcVar8 = pcVar9;
    }
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) == '\0') break;
    do {
      pcVar8 = pcVar8 + 1;
      RT_CALLRA(0x00286d3c, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), cVar4));
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    } while ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0');
    goto LAB_00286e58;
  case 0x43:
    pfVar11 = param_2 + 8;
    // MIPS reads the slot with a plain lw: a guest string pointer (Ghidra typed it float*)
    pcVar9 = (*(uint32_t*)rtguest::G((uint32_t)param_2));
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar9));
    param_2 = pfVar11;
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar9)) == '\0') {
      param_1 = pbVar2 + 2;
      RT_CALLRA(0x00286d84, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x28));
      RT_CALLRA(0x00286d90, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x6e));
      RT_CALLRA(0x00286d9c, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x75));
      RT_CALLRA(0x00286da8, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x6c));
      RT_CALLRA(0x00286db4, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x6c));
      RT_CALLRA(0x00286dc0, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), 0x29));
    }
    else {
      param_1 = pbVar2 + 2;
      do {
        pcVar9 = pcVar9 + 1;
        RT_CALLRA(0x00286dec, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), cVar4));
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar9));
      } while (cVar4 != '\0');
    }
    goto LAB_00286e58;
  case 0x45:
    if (lVar7 == 0x6c) {
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else if (lVar7 == 0x68) {
      uVar10 = (uint64_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else {
      // MIPS reads the va_list slot with an unsigned lwu (Ghidra typed the pointer float*)
      uVar10 = (uint64_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
    }
    param_2 = param_2 + 8;
    pcVar8 = acStack_c0 + 0x4f;
    (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4f) * 1)) = 0;
    if (uVar10 == 0) {
      pcVar8 = acStack_c0 + 0x4e;
      (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4e) * 1)) = 0x30;
    }
    else {
      do {
        cVar4 = RT_CALLRA(0x286ca0, guest::__umoddi3(uVar10,10));
        pcVar8 = pcVar8 + -1;
        (*(int8_t*)rtguest::G((uint32_t)pcVar8)) = cVar4 + '0';
        uVar10 = RT_CALLRA(0x286cbc, guest::__udivdi3(uVar10,10));
      } while (uVar10 != 0);
    }
    param_1 = pbVar2 + 2;
    if ((pcVar9 != (uint32_t)0x0) && (pcVar9 < pcVar8)) {
      pcVar8 = pcVar9;
    }
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0') {
      do {
        pcVar8 = pcVar8 + 1;
        RT_CALLRA(0x00286e20, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), cVar4));
        cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
      } while ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0');
      goto LAB_00286e58;
    }
    break;
  case 0x48:
    if (lVar7 == 0x6c) {
      uVar10 = (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else if (lVar7 == 0x68) {
      uVar10 = (uint64_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)param_2));
    }
    else {
      // MIPS reads the va_list slot with an unsigned lwu (Ghidra typed the pointer float*)
      uVar10 = (uint64_t)(*(uint32_t*)rtguest::G((uint32_t)param_2));
    }
    param_2 = param_2 + 8;
    pcVar8 = acStack_c0 + 0x4f;
    (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4f) * 1)) = 0;
    if (uVar10 == 0) {
      pcVar8 = acStack_c0 + 0x4e;
      (*(int8_t*)rtguest::G((uint32_t)acStack_c0 + (uint32_t)(0x4e) * 1)) = 0x30;
    }
    else {
      do {
        cVar4 = (char)(uVar10 & 0xf);
        cVar3 = cVar4 + '0';
        if (9 < (uVar10 & 0xf)) {
          cVar3 = cVar4 + 'W';
        }
        pcVar8 = pcVar8 + -1;
        uVar10 = uVar10 >> 4;
        (*(int8_t*)rtguest::G((uint32_t)pcVar8)) = cVar3;
      } while (uVar10 != 0);
    }
    param_1 = pbVar2 + 2;
    if ((pcVar9 != (uint32_t)0x0) && (pcVar9 < pcVar8)) {
      pcVar8 = pcVar9;
    }
    cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    if ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) == '\0') break;
    do {
      pcVar8 = pcVar8 + 1;
      RT_CALLRA(0x00286e38, rtCall((*(uint32_t*)rtguest::G(0x002e98f8)), cVar4));
      cVar4 = (*(int8_t*)rtguest::G((uint32_t)pcVar8));
    } while ((*(int8_t*)rtguest::G((uint32_t)pcVar8)) != '\0');
    goto LAB_00286e58;
  default:
    break;
  }
switchD_0028693c_default:
  param_1 = pbVar2 + 2;
  goto LAB_00286e58;
switchD_0028693c_caseD_3c:
  lVar7 = 0x6c;
  goto LAB_00286910;
switchD_0028693c_caseD_1:
  goto switchD_0028693c_default;
}


RT_GUEST_FN(0x002868c0, guest::_printf)

}  // namespace guest
