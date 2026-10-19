// guest:0x0026e4f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceGsSyncPath(uint32_t param_1, ...) {
  bool bVar1;
uint32_t __format;
  uint uVar2;
  ulong uVar3;
  undefined4 in_vc13;
  
  uVar2 = 0;
  if (param_1 != 0) {
    uVar2 = (uint32_t)((REG_DMAC_1_VIF1_CHCR & 0x100) != 0);
    if ((REG_DMAC_2_GIF_CHCR & 0x100) != 0) {
      uVar2 = uVar2 | 2;
    }
    if ((REG_VIF1_STAT & 0x1f000003) != 0) {
      uVar2 = uVar2 | 4;
    }
    uVar3 = _cfc2(in_vc13);
    if ((uVar3 & 0x100) != 0) {
      uVar2 = uVar2 | 8;
    }
    if ((REG_GIF_STAT & 0xc00) == 0) {
    }
    return uVar2 | 0x10;
  }
  do {
    if ((REG_DMAC_1_VIF1_CHCR & 0x100) == 0) goto joined_r0x0026e554;
    bVar1 = uVar2 < 0x1000001;
    uVar2 = uVar2 + 1;
  } while (bVar1);
  __format = 0x32fc48u;
  goto LAB_0026e684;
  while (bVar1 = uVar2 < 0x1000001, uVar2 = uVar2 + 1, bVar1) {
joined_r0x0026e554:
    if ((REG_DMAC_2_GIF_CHCR & 0x100) == 0) goto joined_r0x0026e59c;
  }
  __format = 0x32fd28u;
joined_r0x0026e5dc:
  if ((uVar3 & 0x100) == 0) goto joined_r0x0026e614;
  bVar1 = 0x1000000 < uVar2;
  uVar2 = uVar2 + 1;
  if (bVar1) {
    __format = 0x32fd88u;
  }
  uVar3 = _cfc2(in_vc13);
  goto joined_r0x0026e5dc;
joined_r0x0026e614:
  do {
    if ((REG_GIF_STAT & 0xc00) == 0) {
      return 0;
    }
    bVar1 = uVar2 < 0x1000001;
    uVar2 = uVar2 + 1;
  } while (bVar1);
  __format = 0x32fdb0u;
  while (bVar1 = uVar2 < 0x1000001, uVar2 = uVar2 + 1, bVar1) {
joined_r0x0026e59c:
    if ((REG_VIF1_STAT & 0x1f000003) == 0) {
      uVar3 = _cfc2(in_vc13);
    }
  }
  __format = 0x32fd58u;
LAB_0026e684:
  ::printf((char *)rtguest::G((uint32_t)(__format)));
  ::printf((char *)rtguest::G((uint32_t)(0x32fc78u)), REG_DMAC_1_VIF1_CHCR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fc88u)), REG_DMAC_1_VIF1_TADR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fc98u)), REG_DMAC_1_VIF1_MADR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fca8u)), REG_DMAC_1_VIF1_QWC);
  ::printf((char *)rtguest::G((uint32_t)(0x32fcb8u)), REG_DMAC_2_GIF_CHCR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fcc8u)), REG_DMAC_2_GIF_TADR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fcd8u)), REG_DMAC_2_GIF_MADR);
  ::printf((char *)rtguest::G((uint32_t)(0x32fce8u)), REG_DMAC_2_GIF_QWC);
  ::printf((char *)rtguest::G((uint32_t)(0x32fcf8u)), REG_VIF1_STAT);
  ::printf((char *)rtguest::G((uint32_t)(0x32fd10u)), REG_GIF_STAT);
  return 0xffffffff;
}


RT_GUEST_FN(0x0026e4f0, guest::sceGsSyncPath)

}  // namespace guest
