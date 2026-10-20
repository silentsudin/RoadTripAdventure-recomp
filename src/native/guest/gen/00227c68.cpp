// guest:0x00227c68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitControlsBindings(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  uint uVar1;
uint32_t puVar2;
uint32_t puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  
  iVar4 = 9;
  SetFlagBitFc98(1);
  SetFlagBitFc98(4);
  SetFlagBitFc98(0x5a);
  (*(uint64_t*)rtguest::G(0x177fc68)) = 0xffffffffffffffff;
  puVar2 = 0x0177fd00;
  (*(uint32_t*)rtguest::G(0x177fc70)) = 0xfffffffff;
  (*(uint64_t*)rtguest::G(0x177fc88)) = 0xffffffffffffffff;
  (*(uint32_t*)rtguest::G(0x177fc90)) = 0xfffffffff;
  do {
    iVar4 = iVar4 + -1;
    (*(uint64_t*)rtguest::G((uint32_t)puVar2)) = 0xffffffffffffffff;
    puVar2 = puVar2 + -1;
  } while (-1 < iVar4);
  (*(uint32_t*)rtguest::G(0x177fdb4)) = 10000;
  puVar3 = 0x017803e0;
  (*(int32_t*)rtguest::G(0x1782a6c)) = 100000;
  (*(int8_t*)rtguest::G(0x177fdb1)) = 3;
  (*(uint32_t*)rtguest::G(0x177fd8c)) = (*(uint32_t*)rtguest::G(0x333600));
  (*(uint32_t*)rtguest::G(0x177fd90)) = (*(uint32_t*)rtguest::G(0x333604));
  (*(uint32_t*)rtguest::G(0x177fd9f)) = (*(uint32_t*)rtguest::G(0x333608));
  (*(uint32_t*)rtguest::G(0x177fda3)) = (*(uint32_t*)rtguest::G(0x33360c));
  (*(uint32_t*)rtguest::G(0x178039c)) = 0xffffffff;
  (*(uint32_t*)rtguest::G(0x17803a0)) = 0x005fffff;
  iVar4 = 2;
  (*(uint16_t*)rtguest::G(0x1780a3a)) = 0;
  (*(uint32_t*)rtguest::G(0x1780390)) = 0xffffffff;
  (*(uint32_t*)rtguest::G(0x1780394)) = 0xffffffff;
  (*(uint32_t*)rtguest::G(0x1780398)) = 0xffffffff;
  do {
    iVar4 = iVar4 + -1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-0xf) * sizeof(uint32_t))) = 0x1fff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-10) * sizeof(uint32_t))) = 0xfff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(-5) * sizeof(uint32_t))) = 0x1f;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3 + (uint32_t)(5) * sizeof(uint32_t))) = 0xf;
    (*(uint32_t*)rtguest::G((uint32_t)puVar3)) = 0x3f;
    puVar3 = puVar3 + 0x4;  // T* element step
  } while (-1 < iVar4);
  (*(uint32_t*)rtguest::G(0x1780408)) = 0xf;
  (*(uint32_t*)rtguest::G(0x1780494)) = 0x7fff;
  (*(uint32_t*)rtguest::G(0x1780458)) = 7;
  (*(uint32_t*)rtguest::G(0x1780460)) = 6;
  (*(uint32_t*)rtguest::G(0x178046c)) = 0x1ff;
  (*(uint32_t*)rtguest::G(0x17804a8)) = 0x3bf;
  (*(uint32_t*)rtguest::G(0x1780430)) = 7;
  (*(uint32_t*)rtguest::G(0x178041c)) = 0x7fff;
  (*(uint32_t*)rtguest::G(0x178045c)) = 6;
  (*(uint32_t*)rtguest::G(0x1780480)) = 3;
  (*(uint32_t*)rtguest::G(0x1780444)) = 3;
  iVar5 = 1;
  SetObjectAttribute(0x177f760,3,4);
  SetObjectAttribute(0x177f760,4,4);
  SetObjectAttribute(0x177f760,5,2);
  SetObjectAttribute(0x177f760,6,3);
  SetObjectAttribute(0x177f760,10,2);
  iVar4 = 0x177f77c;
  do {
    iVar5 = iVar5 + -1;
    SetObjectAttribute(iVar4,1,8);
    SetObjectAttribute(iVar4,2,9);
    SetObjectAttribute(iVar4,3,3);
    SetObjectAttribute(iVar4,4,4);
    SetObjectAttribute(iVar4,5,2);
    SetObjectAttribute(iVar4,10,1);
    iVar4 = iVar4 + 0x1c;
  } while (-1 < iVar5);
  SetObjectAttribute(0x177f760,9,1);
  SetObjectAttribute(0x177f760,0xb,2);
  (*(uint8_t*)rtguest::G((uint32_t)0x0177f798 + 4)) = 0x6e;
  (*(int8_t*)rtguest::G(0x177fdd4)) = 6;
  (*(uint32_t*)rtguest::G((uint32_t)0x0177f778 + 4)) = 0x005ac5ac;
  (*(uint8_t*)rtguest::G((uint32_t)0x0177f780 + 0)) = 0x37;
  (*(int8_t*)rtguest::G(0x177fdd6)) = 7;
  (*(uint32_t*)rtguest::G(0x177fdd7)) = 8;
  (*(uint32_t*)rtguest::G((uint32_t)0x0177f798 + 0)) = 0x6f06f;
  (*(uint32_t*)rtguest::G(0x177fdd5)) = 8;
  iVar4 = 1;
  do {
    SetFlaggedSlotBit(iVar4,0,0);
    iVar5 = iVar4 + 1;
    SetFlaggedSlotBit(iVar4,0,0);
    iVar4 = iVar5;
  } while (iVar5 < 8);
  uVar6 = 0;
  uVar1 = 0;
  do {
    uVar6 = uVar6 + 1;
    AppendQueuedByte(uVar1);
    uVar1 = uVar6 & 0xff;
  } while ((int)uVar6 < 0x80);
  return 0;
}


RT_GUEST_FN(0x00227c68, guest::InitControlsBindings)

}  // namespace guest
