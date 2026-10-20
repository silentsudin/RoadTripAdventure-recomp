// guest:0x0023cd58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CheckUnlockCondition(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  bool bVar1;
uint32_t pcVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = (int)param_3;
  uVar4 = 0;
  pcVar2 = (uint32_t)0x2a2b71;
  do {
    if ((param_1 == (*(int8_t*)rtguest::G((uint32_t)pcVar2 + (uint32_t)(-1) * sizeof(int8_t)))) && (param_2 == (*(int8_t*)rtguest::G((uint32_t)pcVar2)))) {
      bVar1 = ((*(uint32_t*)rtguest::G(0x177fdd0)) & 1 << (uVar4 & 0x1f)) != 0;
      goto LAB_0023cda0;
    }
    uVar4 = uVar4 + 1;
    pcVar2 = pcVar2 + 2;
  } while ((int)uVar4 < 0x19);
  bVar1 = false;
LAB_0023cda0:
  if (bVar1) {
    return true;
  }
  if ((param_1 == '\x02') && (param_2 == 0x1d)) {
    if (((*(uint64_t*)rtguest::G(0x177fc98)) & 0x400000) == 0) {
      return false;
    }
    if (0x32898 < param_3) {
      return true;
    }
    return false;
  }
  if (param_1 == '\x06') {
    if (param_2 == 10) {
      if (45000 < iVar3 - 0x1a5e0U) {
        return true;
      }
      return false;
    }
    if (param_2 == 0x11) {
      if (((*(uint32_t*)rtguest::G(0x177fca0)) & 8) == 0) {
        return true;
      }
      return false;
    }
    if (param_2 == 0x20) {
      if ((long)(*(uint64_t*)rtguest::G(0x177fc98)) < 0) {
        return true;
      }
      return false;
    }
  }
  if ((param_1 == '\a') && (param_2 == 10)) {
    bVar1 = ((*(uint64_t*)rtguest::G(0x177fc98)) & 0x100000000000000) != 0;
LAB_0023cf8c:
    if (!bVar1) {
      return true;
    }
  }
  else {
    if (param_1 == '\r') {
      if (param_2 == 0) {
        if (((*(uint32_t*)rtguest::G(0x177fca0)) & 0x200000) == 0) {
          return 0x1c906 < iVar3 - 0xafc9U;
        }
        return false;
      }
      if (param_2 == 5) {
        if (((*(uint32_t*)rtguest::G(0x177fca0)) & 0x200000) != 0) {
          return true;
        }
        bVar1 = 0x1c906 < iVar3 - 0xafc9U;
        goto LAB_0023cf8c;
      }
    }
    if (param_1 == '\t') {
      switch((int)param_2) {
      case 0x17:
        return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x8000) == 0;
      case 0x18:
        uVar4 = 0x10000;
        break;
      case 0x19:
        return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x2000) == 0;
      case 0x1a:
        return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x4000) == 0;
      case 0x1b:
        return ((*(uint32_t*)rtguest::G(0x177fdd0)) & 0x1000) == 0;
      case 0x1c:
        uVar4 = 0x20000;
        break;
      default:
        goto switchD_0023cfc8_default;
      }
      return ((*(uint32_t*)rtguest::G(0x177fdd0)) & uVar4) == 0;
    }
  }
switchD_0023cfc8_default:
  return false;
}


RT_GUEST_FN(0x0023cd58, guest::CheckUnlockCondition)

}  // namespace guest
