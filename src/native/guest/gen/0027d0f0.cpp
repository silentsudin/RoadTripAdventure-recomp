// guest:0x0027d0f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t execute_cfa_insn(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
    uint32_t iStack_30 = rtguest::allocFrame(4);
    uint32_t uStack_2c = rtguest::allocFrame(4);
  byte bVar1;
  uint uVar2;
uint32_t puVar3;
  undefined8 uVar4;
uint32_t puVar5;
uint32_t puVar6;
  uint uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
uint32_t puVar10;
uint32_t puVar11;
uint32_t auStack_28 = rtguest::allocFrame(8);
  
  bVar1 = (*(int8_t*)rtguest::G((uint32_t)param_1));
  puVar10 = (uint32_t)(param_1 + 1);
  if ((bVar1 & 0x40) != 0) {
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)param_4));
    uVar2 = (bVar1 & 0x3f) * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 8)));
LAB_0027d1dc:
    (*(uint32_t*)rtguest::G((uint32_t)param_4)) = uVar7 + uVar2;
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  }
  if ((bVar1 & 0x80) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = bVar1 & 0x3f;
LAB_0027d21c:
    puVar10 = (uint32_t)decode_uleb128(puVar10,iStack_30);
    if ((*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) != (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x7f)))) {
      (*(int32_t*)rtguest::G((uint32_t)iStack_30)) = (*(int32_t*)rtguest::G((uint32_t)iStack_30)) * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0xc)));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 1;
      (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(uStack_2c + 4) * sizeof(uint64_t))) = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_30));
      puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
      return puVar10;
    }
    goto code_r0x0027d500;
  }
  if ((bVar1 & 0xc0) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = bVar1 & 0x3f;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 0;
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  }
  switch(bVar1) {
  case 1:
    (*(uint32_t*)rtguest::G((uint32_t)param_4)) = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 2:
    uVar7 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar10));
    goto LAB_0027d1f4;
  case 3:
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)param_4));
    uVar2 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar10));
    goto LAB_0027d1dc;
  case 4:
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)puVar10));
LAB_0027d1f4:
    (*(uint32_t*)rtguest::G((uint32_t)param_4)) = (*(uint32_t*)rtguest::G((uint32_t)param_4)) + uVar7;
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 5:
    puVar10 = (uint32_t)decode_uleb128(puVar10,uStack_2c);
    goto LAB_0027d21c;
  case 6:
    decode_uleb128(puVar10,uStack_2c);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 0;
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 9:
    uVar4 = decode_uleb128(puVar10,uStack_2c);
    decode_uleb128(uVar4,auStack_28);
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 2;
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(uStack_2c + 4) * sizeof(uint64_t))) = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)auStack_28 + (uint32_t)(0) * 4));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 10:
    puVar3 = malloc(0x480);
    puVar6 = puVar3;
    puVar5 = param_2;
    do {
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t)));
      uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t)));
      uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)puVar6)) = (*(uint64_t*)rtguest::G((uint32_t)puVar5));
      (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(1) * sizeof(uint64_t))) = uVar4;
      (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(2) * sizeof(uint64_t))) = uVar8;
      (*(uint64_t*)rtguest::G((uint32_t)puVar6 + (uint32_t)(3) * sizeof(uint64_t))) = uVar9;
      puVar5 = puVar5 + 0x20;  // T* element step
      puVar6 = puVar6 + 0x20;  // T* element step
    } while (puVar5 != param_2 + 0x90);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x8f))) = puVar3;
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 0xb:
    puVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x8f)));
    puVar5 = puVar6;
    do {
      uVar4 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(1) * sizeof(uint64_t)));
      uVar8 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(2) * sizeof(uint64_t)));
      uVar9 = (*(uint64_t*)rtguest::G((uint32_t)puVar5 + (uint32_t)(3) * sizeof(uint64_t)));
      (*(uint64_t*)rtguest::G((uint32_t)param_2)) = (*(uint64_t*)rtguest::G((uint32_t)puVar5));
      (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint64_t))) = uVar4;
      (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint64_t))) = uVar8;
      (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint64_t))) = uVar9;
      puVar5 = puVar5 + 0x20;  // T* element step
      param_2 = param_2 + 4;
    } while (puVar5 != puVar6 + 0x90);
    free(puVar6);
    break;
  case 0xc:
    uVar4 = decode_uleb128(puVar10,uStack_2c);
    decode_uleb128(uVar4,iStack_30);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x7f))) = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_2c));
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint64_t))) = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_30));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 0xd:
    decode_uleb128(puVar10,uStack_2c);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x7f))) = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)uStack_2c));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 0xe:
    decode_uleb128(puVar10,iStack_30);
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint64_t))) = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_30));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 0xf:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x7f))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(1) * sizeof(uint64_t))) = 0;
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(2) * sizeof(uint64_t))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x477))) = 0;
    puVar10 = (uint32_t)decode_uleb128(puVar10,iStack_30);
    puVar11 = (uint32_t)((int)puVar10 + (*(int32_t*)rtguest::G((uint32_t)iStack_30)));
    for (; puVar10 < puVar11; puVar10 = (uint32_t)decode_stack_op(puVar10,param_2)) {
    }
    break;
  default:
    abort(puVar10);
  case 0:
  case 7:
  case 8:
code_r0x0027d500:
    break;
  case 0x2d:
    (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = 0x10;
    do {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 1;
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 1;
      (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(uStack_2c + 4) * sizeof(uint64_t))) = (uint32_t)(((*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) - 0x10) * 4);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) = uVar7;
    } while (uVar7 < 0x20);
    break;
  case 0x2e:
    decode_uleb128(puVar10,iStack_30);
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(3) * sizeof(uint64_t))) = (long)(*(int32_t*)rtguest::G((uint32_t)iStack_30));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  case 0x2f:
    uVar4 = decode_uleb128(puVar10,uStack_2c);
    decode_uleb128(uVar4,iStack_30);
    (*(int32_t*)rtguest::G((uint32_t)iStack_30)) = (*(int32_t*)rtguest::G((uint32_t)iStack_30)) * (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_3 + 0xc)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + (*(uint32_t*)rtguest::G((uint32_t)uStack_2c)) + 0x3fc))) = 1;
    (*(uint64_t*)rtguest::G((uint32_t)param_2 + (uint32_t)(uStack_2c + 4) * sizeof(uint64_t))) = (long)-(*(int32_t*)rtguest::G((uint32_t)iStack_30));
    puVar10 = (uint32_t)switchD_0027d18c_executeCfaInsnEpilogue();
    return puVar10;
  }
  return puVar10;
}


RT_GUEST_FN(0x0027d0f0, guest::execute_cfa_insn)

}  // namespace guest
