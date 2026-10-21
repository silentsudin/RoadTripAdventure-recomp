// guest:0x00286f98 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifInitCmd(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
  }
uint32_t puVar1;
  long lVar2;
  ulong uVar3;
  undefined8 uVar4;
  int iVar5;
  
  RT_CALLRA(0x286fb0, guest::DIntr());
  if ((*(int32_t*)rtguest::G(0x2e98fc)) != 0) {
    EIntr();
    return 0;
  }
  (*(int32_t*)rtguest::G(0x2e98fc)) = 1;
  puVar1 = 0x01776ec0;
  (*(uint32_t*)rtguest::G(0x1776e98)) = 0x21776dc0;
  (*(uint32_t*)rtguest::G(0x1776eb4)) = 0x01776fc0;
  (*(uint32_t*)rtguest::G(0x1776e9c)) = 0x21776e40;
  (*(uint32_t*)rtguest::G(0x1776ea8)) = 0x20;
  (*(uint32_t*)rtguest::G(0x1776ea0)) = 0;
  iVar5 = 0x1f;
  (*(int32_t*)rtguest::G(0x1776ea4)) = 0x01776ec0;
  (*(uint32_t*)rtguest::G(0x1776eac)) = 0;
  (*(uint32_t*)rtguest::G(0x1776eb0)) = 0;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 0;
    iVar5 = iVar5 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar1 + (uint32_t)(1) * sizeof(uint32_t))) = 0;
    puVar1 = puVar1 + (uint32_t)(2) * sizeof(uint32_t);
  } while (-1 < iVar5);
  iVar5 = 0x1f;
  puVar1 = 0x0177703c;
  do {
    (*(uint32_t*)rtguest::G((uint32_t)puVar1)) = 0;
    iVar5 = iVar5 + -1;
    puVar1 = puVar1 + (uint32_t)(-1) * sizeof(uint32_t);
  } while (-1 < iVar5);
  (*(uint32_t*)rtguest::G(0x1776ec0)) = 0x00286f40;
  (*(uint32_t*)rtguest::G(0x1776ec8)) = 0x00286f20;
  (*(uint32_t*)rtguest::G(0x1776ecc)) = 0x01776e98;
  (*(uint32_t*)rtguest::G(0x1776ec4)) = 0x01776e98;
  EIntr();
  RT_CALLRA(0x2870cc, guest::FlushCache(0));
  if ((REG_DMAC_STAT & 0x20) != 0) {
    REG_DMAC_STAT = 0x20;
  }
  if ((REG_DMAC_5_SIF0_CHCR & 0x100) == 0) {
    RT_CALLRA(0x28710c, guest::sceSifSetDChain());
  }
  (*(uint32_t*)rtguest::G(0x1776e94)) = RT_CALLRA(0x287120, guest::AddDmacHandler(5,0x287490,0));
  RT_CALLRA(0x287130, guest::enableDmac(5));
  lVar2 = RT_CALLRA(0x28713c, guest::sceSifGetReg(0xffffffff80000000));
  (*(uint32_t*)rtguest::G(0x1776ea0)) = (uint32_t)lVar2;
  if (lVar2 != 0) {
    (*(uint32_t*)rtguest::G(0x1776e90)) = 0x01776dc0;
    sceSifSendCmd(0xffffffff80000000,0x1776e80,0x14,0,0,0);
    return 0;
  }
  (*(uint32_t*)rtguest::G(0x1776ea0)) = 0;
  do {
    uVar3 = RT_CALLRA(0x287190, guest::sceSifGetReg(4));
  } while ((uVar3 & 0x20000) == 0);
  uVar4 = RT_CALLRA(0x2871a4, guest::sceSifGetReg(2));
  (*(uint32_t*)rtguest::G(0x1776ea0)) = (uint32_t)uVar4;
  RT_CALLRA(0x2871b4, guest::sceSifSetReg(0xffffffff80000000,uVar4));
  RT_CALLRA(0x2871c4, guest::sceSifSetReg(0xffffffff80000001,0x1776e98));
  (*(uint32_t*)rtguest::G(0x1776e90)) = 0x01776dc0;
  (*(uint32_t*)rtguest::G(0x1776e8c)) = 0;
  sceSifSendCmd(0xffffffff80000002,0x1776e80,0x14,0,0,0);
  return 0;
}


RT_GUEST_FN(0x00286f98, guest::sceSifInitCmd)

}  // namespace guest
