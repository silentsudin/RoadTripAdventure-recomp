// guest:0x00285e00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t InitThread(uint32_t q0, ...) {
uint32_t auStack_80 = rtguest::allocFrame(4);
rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_74 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_6c = rtguest::allocFrame(4);
    rtguest::allocFrame(24);  // guest frame gap
uint32_t auStack_50 = rtguest::allocFrame(4);
    uint32_t uStack_4c = rtguest::allocFrame(4);
    uint32_t uStack_48 = rtguest::allocFrame(4);
  long lVar1;
  undefined8 uVar2;
uint32_t pcStack_7c;
uint32_t puStack_78;
uint32_t puStack_70;
  
  if ((*(int32_t*)rtguest::G(0x2e98f0)) < 1) {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_48)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_4c)) = 0xff;
    lVar1 = CreateSema(auStack_50);
    (*(uint32_t*)rtguest::G(0x1776570)) = (uint32_t)lVar1;
    if (-1 < lVar1) {
      pcStack_7c = 0x285d28;
      puStack_78 = 0x01776170;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = 0x400;
      puStack_70 = 0x33aaf0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_6c)) = 0;
      lVar1 = CreateThread(auStack_80);
      (*(int32_t*)rtguest::G(0x2e98f0)) = (int)lVar1;
      if (-1 < lVar1) {
        (*(uint32_t*)rtguest::G(0x1776578)) = 0;
        (*(uint32_t*)rtguest::G(0x177657c)) = 0;
        StartThread(lVar1,0x1776578);
        uVar2 = GetThreadId();
        ChangeThreadPriority(uVar2,1);
        return (*(int32_t*)rtguest::G(0x2e98f0));
      }
      DeleteSema((*(uint32_t*)rtguest::G(0x1776570)));
    }
  }
  return -1;
}


RT_GUEST_FN(0x00285e00, guest::InitThread)

}  // namespace guest
