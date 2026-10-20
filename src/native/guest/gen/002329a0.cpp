// guest:0x002329a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetVehicleMode(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined4 uVar1;
uint32_t puVar2;
uint32_t __s;
  
  __s = (uint32_t)(param_1 + 0x800c);
  if ((*(int32_t*)rtguest::G((uint32_t)__s)) != param_2) {
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x6c));
    (*(int32_t*)rtguest::G((uint32_t)__s)) = param_2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8010))) = 0x140;
    uVar1 = 0x40;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8004))) != 0) {
      uVar1 = 0xe0;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8014))) = uVar1;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8008))) == 0x1e) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8014))) = 0xe0;
    }
    puVar2 = (uint32_t)(param_1 + 0x8018);
    switch(param_2) {
    case 1:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 9;
      break;
    case 2:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8038))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8044))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8040))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x803c))) = 1;
      break;
    case 3:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8038))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8058))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8064))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8060))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x805c))) = 0x80;
      break;
    case 4:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x803c))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x805c))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8064))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8060))) = 0x80;
      break;
    case 5:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 9;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8040))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8060))) = 0x80;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8064))) = 0x80;
      break;
    case 6:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8028))) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8024))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8020))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x801c))) = 7;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8034))) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8030))) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x802c))) = 8;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8048))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8044))) = 2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8054))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8050))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x804c))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8064))) = 0x80;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002329a0, guest::SetVehicleMode)

}  // namespace guest
