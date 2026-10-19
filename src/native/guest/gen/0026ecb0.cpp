// guest:0x0026ecb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsExecStoreImage(uint32_t param_1, ...) {
  bool bVar1;
  uint uVar2;
  ulong uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  uVar8 = 0;
  uVar7 = 0;
  uVar2 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40)));
  uVar6 = uVar2 & 0xfff;
  uVar5 = (uint32_t)((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40))) >> 0x20) & 0xfff;
  uVar4 = 0;
  switch((uint32_t)((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20))) >> 0x18) & 0x3f) {
  case 0:
  case 0x30:
    uVar8 = uVar6 * uVar5 * 4 & 0xf;
    uVar4 = uVar5;
    if (uVar8 != 0) {
      uVar4 = uVar5 + 3 & 0x1ffc;
    }
    break;
  case 1:
  case 0x31:
    uVar8 = uVar6 * uVar5 * 3 & 0xf;
    uVar4 = uVar5;
    if (uVar8 != 0) {
      uVar4 = uVar5 + 0xf & 0x1ff0;
    }
    break;
  case 2:
  case 10:
  case 0x32:
  case 0x3a:
    uVar8 = uVar6 * uVar5 * 2 & 0xf;
    uVar4 = uVar5;
    if (uVar8 != 0) {
      uVar4 = uVar5 + 7 & 0xfffffff8;
    }
    break;
  case 0x13:
  case 0x1b:
    uVar8 = uVar6 * uVar5 & 0xf;
    uVar4 = uVar5;
    if (uVar8 != 0) {
      uVar4 = uVar5 + 7 & 0xfffffff8;
    }
    break;
  case 0x14:
  case 0x24:
  case 0x2c:
    uVar8 = (int)(uVar6 * uVar5) >> 1 & 0xf;
    uVar4 = uVar5;
    if (uVar8 != 0) {
      uVar4 = uVar5 + 7 & 0xfffffff8;
    }
  }
  if (uVar8 != 0) {
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40 | 0x20000000))) = (long)(int)(uVar2 & 0xfff) | (uint32_t)uVar4 << 0x20;
  }
  while ((REG_DMAC_1_VIF1_CHCR & 0x100) != 0) {
    bVar1 = 0x1000000 < uVar7;
    uVar7 = uVar7 + 1;
    if (bVar1) goto LAB_0026f058;
  }
  uVar3 = GsGetIMR();
  GsPutIMR(uVar3 | 0x200);
  (*(uint8_t*)rtguest::G(0x12001000)) = 2;
  REG_DMAC_1_VIF1_QWC = 7;
  if ((param_1 & 0x70000000) == 0x70000000) {
    REG_DMAC_1_VIF1_MADR = param_1 & 0xfffffff | 0x80000000;
  }
  else {
    REG_DMAC_1_VIF1_MADR = param_1 & 0xfffffff;
  }
  REG_DMAC_1_VIF1_CHCR = 0x101;
  do {
    bVar1 = uVar7 < 0x1000001;
    uVar7 = uVar7 + 1;
  } while (bVar1);
LAB_0026f058:
  ::printf((char *)rtguest::G((uint32_t)(0x32ff40u)));
  return 0xffffffff;
}


RT_GUEST_FN(0x0026ecb0, guest::sceGsExecStoreImage)

}  // namespace guest
