// guest:0x0026ecb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsExecStoreImage(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(144);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x80) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x84) = 0x00000000u;
    rtguest::gS[0] = 0x0000000000000000ull;  // walk const
    rtguest::gS[1] = 0x0000000000000000ull;  // walk const
    rtguest::gS[2] = 0x0000000000000000ull;  // walk const
    rtguest::gS[3] = 0x0000000000000000ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[6] = 0x0000000000000000ull;  // walk const
  }
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
  uVar5 = (uint32_t)((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40))) >> 0x20) & 0xfff;
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
    (*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x40 | 0x20000000))) = (long)(int)(uVar2 & 0xfff) | (uint64_t)uVar4 << 0x20;
  }
  while ((REG_DMAC_1_VIF1_CHCR & 0x100) != 0) {
    bVar1 = 0x1000000 < uVar7;
    uVar7 = uVar7 + 1;
    if (bVar1) goto LAB_0026f058;
  }
  uVar3 = RT_CALLRA(0x26eef4, guest::GsGetIMR());
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
  rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x32ff40u)));
  return 0xffffffff;
}


RT_GUEST_FN(0x0026ecb0, guest::sceGsExecStoreImage)

}  // namespace guest
